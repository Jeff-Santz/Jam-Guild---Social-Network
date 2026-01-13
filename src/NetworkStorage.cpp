#include "NetworkStorage.h"
#include "ImagePost.h" 
#include "Page.h" 
#include "User.h"
#include "Notification.h"
#include <iostream>
#include <map>

using namespace std;

NetworkStorage::NetworkStorage(string dbFileName) {
    this->dbFileName = dbFileName;
    this->db = nullptr;

    int exit = sqlite3_open(dbFileName.c_str(), &db);
    if (exit != SQLITE_OK) cerr << "Error opening DB" << endl;
    else cout << "Database opened successfully!" << endl;

    executeSQL("PRAGMA journal_mode=WAL;");

    // Atualizações de esquema (migrations) podem ser gerenciadas aqui
    // executeSQL("ALTER TABLE PROFILES ADD COLUMN IS_VERIFIED INT DEFAULT 0;");

    // 1. Tabela PROFILES 
    // ADICIONEI A COLUNA 'IS_VERIFIED' NO FINAL
    string sqlUsers = "CREATE TABLE IF NOT EXISTS PROFILES("
                      "ID INT PRIMARY KEY NOT NULL, "
                      "NAME TEXT NOT NULL, "
                      "TYPE TEXT NOT NULL, "
                      "EMAIL TEXT, "
                      "OWNER_ID INT, "
                      "PASSWORD TEXT, "
                      "ICON_PATH TEXT, "
                      "BIO TEXT, "
                      "SUBTITLE TEXT, "   
                      "STARTDATE TEXT, "
                      "IS_VERIFIED INT );"; // 0 ou 1
    executeSQL(sqlUsers);

    // 2. Conexões
    executeSQL("CREATE TABLE IF NOT EXISTS CONNECTIONS(ID1 INT NOT NULL, ID2 INT NOT NULL);");
    
    // 3. Posts
    string sqlPosts = "CREATE TABLE IF NOT EXISTS POSTS("
                      "TEXT TEXT NOT NULL, "
                      "DATE BIGINT NOT NULL, "
                      "AUTHOR_ID INT NOT NULL, "
                      "TYPE TEXT, "
                      "MEDIA_PATH TEXT);";
    executeSQL(sqlPosts);

    // 4. Curtidas
    string sqlLikes = "CREATE TABLE IF NOT EXISTS LIKES("
                      "POST_ID INT NOT NULL, "
                      "PROFILE_ID INT NOT NULL);";
    executeSQL(sqlLikes);

    // 5. Comentários
    string sqlComments = "CREATE TABLE IF NOT EXISTS COMMENTS("
                      "POST_ID INT NOT NULL, "
                      "AUTHOR_ID INT NOT NULL, "
                      "TEXT TEXT NOT NULL, "
                      "DATE BIGINT NOT NULL);";
    executeSQL(sqlComments);

    // 6. Pedidos de Amizade (Pendentes)
    string sqlRequests = "CREATE TABLE IF NOT EXISTS FRIEND_REQUESTS("
                        "SENDER_ID INT NOT NULL, "
                        "RECEIVER_ID INT NOT NULL);";
    executeSQL(sqlRequests);

    //7. Notificações
    string sqlNotifications = "CREATE TABLE IF NOT EXISTS NOTIFICATIONS("
                              "USER_ID INT NOT NULL, "
                              "MESSAGE TEXT NOT NULL);";
    executeSQL(sqlNotifications);
}

NetworkStorage::~NetworkStorage() {
    sqlite3_close(db);
    cout << "Database connection closed." << endl;
}

void NetworkStorage::executeSQL(string sql) {
    char* zErrMsg = 0;
    int rc = sqlite3_exec(db, sql.c_str(), nullptr, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
        cerr << "SQL Error: " << zErrMsg << endl;
        sqlite3_free(zErrMsg);
    }
}

void NetworkStorage::save(SocialNetwork* sn) {
    cout << "DEBUG: Save iniciado." << endl;
    
    // --- PARTE 1: PERFIS ---
    sqlite3_exec(db, "DELETE FROM PROFILES;", nullptr, 0, nullptr);
    
    const auto& profiles = sn->getProfiles();

    for (const auto& u_ptr : profiles) {
        int ownerId = -1;
        
        // Verifica se é Página para pegar o ID do dono
        Page* pagePtr = dynamic_cast<Page*>(u_ptr.get());
        if (pagePtr != nullptr && pagePtr->getOwner() != nullptr) {
            ownerId = pagePtr->getOwner()->getId();
        }

        // Verifica se é Usuário para pegar Email e Status
        string email = "";
        int isVerifiedInt = 0;
        User* userPtr = dynamic_cast<User*>(u_ptr.get());
        
        if (userPtr) {
            email = userPtr->getEmail(); // Usando seu getEmail()
            isVerifiedInt = userPtr->isVerified() ? 1 : 0;
        }

        string sql = "INSERT INTO PROFILES VALUES (";
        sql += to_string(u_ptr->getId()) + ", ";
        sql += "'" + u_ptr->getName() + "', ";
        sql += "'" + u_ptr->getRole() + "', ";
        sql += "'" + email + "', "; 
        sql += to_string(ownerId) + ", ";
        sql += "'" + u_ptr->getPassword() + "', ";
        sql += "'" + u_ptr->getIconPath() + "', ";
        sql += "'" + u_ptr->getBio() + "', ";
        sql += "'" + u_ptr->getSubtitle() + "', "; 
        sql += "'" + u_ptr->getStartDate() + "', ";
        sql += to_string(isVerifiedInt) + ");"; // Salva o booleano (0 ou 1)

        sqlite3_exec(db, sql.c_str(), nullptr, 0, nullptr);
    }

    // --- PARTE 2: CONEXOES ---
    sqlite3_exec(db, "DELETE FROM CONNECTIONS;", nullptr, 0, nullptr);
    for (const auto& u_ptr : profiles) {
        vector<Profile*>* contacts = u_ptr->getContacts();
        for (Profile* contact : *contacts) {
            string sql = "INSERT INTO CONNECTIONS VALUES (" + 
                         to_string(u_ptr->getId()) + ", " + 
                         to_string(contact->getId()) + ");";
            sqlite3_exec(db, sql.c_str(), nullptr, 0, nullptr);
        }
    }

    // --- PARTE 3: POSTS, LIKES E COMENTARIOS ---
    sqlite3_exec(db, "DELETE FROM POSTS;", nullptr, 0, nullptr);
    sqlite3_exec(db, "DELETE FROM LIKES;", nullptr, 0, nullptr);
    sqlite3_exec(db, "DELETE FROM COMMENTS;", nullptr, 0, nullptr);
    
    int postGlobalId = 0; 
    for (const auto& u_ptr : profiles) {
        for (Post* p : *u_ptr->getPosts()) {
            postGlobalId++; 

            // Salva o Post - Especificando as colunas para não ter erro
            string sqlPost = "INSERT INTO POSTS (ROWID, TEXT, DATE, AUTHOR_ID, TYPE, MEDIA_PATH) VALUES (" +
                             to_string(postGlobalId) + ", '" + p->getText() + "', " +
                             to_string((long long)p->getDate()) + ", " + to_string(u_ptr->getId()) + ", " +
                             "'" + p->getType() + "', '" + p->getMediaPath() + "');";
            sqlite3_exec(db, sqlPost.c_str(), nullptr, 0, nullptr);

            // Salva as Curtidas (Usando o método que retorna o vector)
            for (Profile* liker : p->getLikes()) { 
                string sqlLike = "INSERT INTO LIKES VALUES (" + to_string(postGlobalId) + ", " + to_string(liker->getId()) + ");";
                sqlite3_exec(db, sqlLike.c_str(), nullptr, 0, nullptr);
            }

            // Salva os Comentários
            for (Comment* c : p->getComments()) {
                string sqlCmt = "INSERT INTO COMMENTS VALUES (" +
                                to_string(postGlobalId) + ", " + to_string(c->getAuthor()->getId()) + ", '" +
                                c->getText() + "', " + to_string((long long)std::time(0)) + ");";
                sqlite3_exec(db, sqlCmt.c_str(), nullptr, 0, nullptr);
            }
        }
    }

    // --- PARTE 4: SOLICITACOES DE AMIZADE ---
    sqlite3_exec(db, "DELETE FROM FRIEND_REQUESTS;", nullptr, 0, nullptr);
    for (const auto& u_ptr : profiles) {
        // Pegamos a lista de quem enviou pedido para o perfil atual
        auto& requests = u_ptr->getContactRequests(); 
        for (Profile* sender : requests) {
            string sql = "INSERT INTO FRIEND_REQUESTS VALUES (" + 
                        to_string(sender->getId()) + ", " + 
                        to_string(u_ptr->getId()) + ");";
            sqlite3_exec(db, sql.c_str(), nullptr, 0, nullptr);
        }
    }

    // --- PARTE 5: NOTIFICAÇÕES ---
    executeSQL("DELETE FROM NOTIFICATIONS;");

    for (auto& profile : sn->getProfiles()) {
        User* uPtr = dynamic_cast<User*>(profile.get());
        if (uPtr) {
            for (const auto& n : uPtr->getNotifications()) {
                string sql = "INSERT INTO NOTIFICATIONS (USER_ID, MESSAGE) VALUES (" +
                            to_string(uPtr->getId()) + ", '" + n->getMessage() + "');";
                executeSQL(sql);
            }
        }
    }

    cout << "DEBUG: Save concluido." << endl;
}

void NetworkStorage::load(SocialNetwork* sn) {
    cout << "DEBUG: Loading..." << endl;
    
    map<int, int> pendingOwners; 

    // --- FASE 1: PERFIS ---
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, "SELECT * FROM PROFILES", -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            // Leitura das colunas
            int id = sqlite3_column_int(stmt, 0);
            string name = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
            string type = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)));
            
            string email = "";
            if(sqlite3_column_text(stmt, 3))
                email = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3)));

            int ownerId = sqlite3_column_int(stmt, 4);

            string password = "";
            if(sqlite3_column_text(stmt, 5))
                password = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5)));

            string icon = "default.png";
            if(sqlite3_column_text(stmt, 6))
                icon = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6)));

            string bio = "";
            if(sqlite3_column_text(stmt, 7))
                bio = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7)));

            string subtitle = "Indefinido";
            if(sqlite3_column_text(stmt, 8))
                subtitle = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 8)));

            string startDate = "01/01/2000";
            if(sqlite3_column_text(stmt, 9))
                startDate = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 9)));

            // Nova Coluna (Indice 10)
            int isVerifiedVal = 0;
            if(sqlite3_column_type(stmt, 10) != SQLITE_NULL)
                isVerifiedVal = sqlite3_column_int(stmt, 10);
            bool isVerified = (isVerifiedVal == 1);

            Profile* p = nullptr;

            if (type == "PAGE") {
                // Passamos o ID, Nome, Dono(null), Icon, Bio, Subtitle, Date
                // NÃO PASSAMOS MAIS A SENHA AQUI
                p = new Page(id, name, nullptr, icon, bio, subtitle, startDate);
                
                if (ownerId != -1) pendingOwners[id] = ownerId;
            }
            else {
                // User continua normal
                p = new User(name, password, icon, bio, subtitle, startDate, isVerified, email);
            }

            if (p != nullptr) {
                p->setId(id);
                sn->add(p);
            }
        }
    }
    sqlite3_finalize(stmt);
    
    // --- FASE EXTRA: RECONECTAR DONOS ---
    for (auto const& [pageId, ownerId] : pendingOwners) {
        try {
            Page* pg = dynamic_cast<Page*>(sn->getProfile(pageId));
            User* owner = dynamic_cast<User*>(sn->getProfile(ownerId));
            if (pg && owner) {
                pg->setOwner(owner);
            }
        } catch (...) {}
    }

    // --- FASE 2: CONEXOES ---
    sqlite3_stmt* stmtRel;
    if (sqlite3_prepare_v2(db, "SELECT * FROM CONNECTIONS", -1, &stmtRel, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmtRel) == SQLITE_ROW) {
            try {
                int id1 = sqlite3_column_int(stmtRel, 0);
                int id2 = sqlite3_column_int(stmtRel, 1);
                Profile* p1 = sn->getProfile(id1);
                Profile* p2 = sn->getProfile(id2);
                if (p1 && p2) p1->addContact(p2);
            } catch (...) {}
        }
    }
    sqlite3_finalize(stmtRel);

    // --- FASE 3: POSTS ---
    sqlite3_stmt* stmtPost;
    if (sqlite3_prepare_v2(db, "SELECT ROWID, * FROM POSTS", -1, &stmtPost, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmtPost) == SQLITE_ROW) {
            int postDbId = sqlite3_column_int(stmtPost, 0);
            string txt = string((const char*)sqlite3_column_text(stmtPost, 1));
            time_t date = (time_t)sqlite3_column_int64(stmtPost, 2);
            int authorId = sqlite3_column_int(stmtPost, 3);
            string type = string((const char*)sqlite3_column_text(stmtPost, 4));
            string media = string((const char*)sqlite3_column_text(stmtPost, 5));

            try {
                Profile* author = sn->getProfile(authorId);
                if (author) {
                    Post* newPost;
                    if (type == "IMAGE") newPost = new ImagePost(txt, date, author, media);
                    else newPost = new Post(txt, date, author);

                    // Curtidas
                    sqlite3_stmt* stmtLike;
                    string sqlL = "SELECT PROFILE_ID FROM LIKES WHERE POST_ID = " + to_string(postDbId);
                    if (sqlite3_prepare_v2(db, sqlL.c_str(), -1, &stmtLike, nullptr) == SQLITE_OK) {
                        while (sqlite3_step(stmtLike) == SQLITE_ROW) {
                            try {
                                Profile* liker = sn->getProfile(sqlite3_column_int(stmtLike, 0));
                                if (liker) newPost->addLike(liker);
                            } catch (...) {}
                        }
                        sqlite3_finalize(stmtLike);
                    }

                    // Comentários
                    sqlite3_stmt* stmtCmt;
                    string sqlC = "SELECT AUTHOR_ID, TEXT FROM COMMENTS WHERE POST_ID = " + to_string(postDbId);
                    if (sqlite3_prepare_v2(db, sqlC.c_str(), -1, &stmtCmt, nullptr) == SQLITE_OK) {
                        while (sqlite3_step(stmtCmt) == SQLITE_ROW) {
                            try {
                                Profile* cmtAuthor = sn->getProfile(sqlite3_column_int(stmtCmt, 0));
                                string cmtTxt = string((const char*)sqlite3_column_text(stmtCmt, 1));
                                if (cmtAuthor) newPost->addComment(cmtTxt, cmtAuthor);
                            } catch (...) {}
                        }
                        sqlite3_finalize(stmtCmt);
                    }
                    author->addPost(newPost);
                }
            } catch (...) {}
        }
    }
    sqlite3_finalize(stmtPost);

     // --- FASE 4: PEDIDOS DE AMIZADE ---
    sqlite3_stmt* stmtReq;
    if (sqlite3_prepare_v2(db, "SELECT * FROM FRIEND_REQUESTS", -1, &stmtReq, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmtReq) == SQLITE_ROW) {
            try {
                int senderId = sqlite3_column_int(stmtReq, 0);
                int receiverId = sqlite3_column_int(stmtReq, 1);
                Profile* sender = sn->getProfile(senderId);
                Profile* receiver = sn->getProfile(receiverId);
                if (sender && receiver) receiver->addContactRequest(sender);
            } catch (...) {}
        }
    }
    sqlite3_finalize(stmtReq);

    // --- FASE 5: NOTIFICAÇÕES ---
    sqlite3_stmt* stmtNotif;
    if (sqlite3_prepare_v2(db, "SELECT USER_ID, MESSAGE FROM NOTIFICATIONS", -1, &stmtNotif, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmtNotif) == SQLITE_ROW) {
            try {
                int userId = sqlite3_column_int(stmtNotif, 0);
                string message = string((const char*)sqlite3_column_text(stmtNotif, 1));
                
                Profile* profile = sn->getProfile(userId);
                User* user = dynamic_cast<User*>(profile);
                if (user) user->addNotification(new Notification(message));
            } catch (...) {}
        }
        sqlite3_finalize(stmtNotif); 
    }
    
    cout << "Data loaded successfully!" << endl;
}

void NetworkStorage::saveUser(Profile* p) {
    std::string sql = "INSERT INTO Profiles (id, name, type, password) VALUES (?, ?, ?, ?);";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("Erro SQL Prepare: " + std::string(sqlite3_errmsg(db)));
    }

    sqlite3_bind_int(stmt, 1, p->getId());
    sqlite3_bind_text(stmt, 2, p->getName().c_str(), -1, SQLITE_STATIC);
    
    std::string role = p->getRole();
    std::string typeStr = (role == "PAGE") ? "PAGE" : "USER";
    sqlite3_bind_text(stmt, 3, typeStr.c_str(), -1, SQLITE_STATIC);

    // Bind da Senha
    sqlite3_bind_text(stmt, 4, p->getPassword().c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        std::string err = sqlite3_errmsg(db);
        sqlite3_finalize(stmt);
        throw std::runtime_error("Erro ao inserir no banco: " + err);
    }

    sqlite3_finalize(stmt);
}
#include "NetworkStorage.h"
#include "ImagePost.h" 
#include "Page.h" 
#include <iostream>
#include <map>

using namespace std;

NetworkStorage::NetworkStorage(string dbFileName) {
    this->dbFileName = dbFileName;
    this->db = nullptr;

    int exit = sqlite3_open(dbFileName.c_str(), &db);
    if (exit != SQLITE_OK) cerr << "Error opening DB" << endl;
    else cout << "Database opened successfully!" << endl;

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

    // --- PARTE 3: POSTS ---
    sqlite3_exec(db, "DELETE FROM POSTS;", nullptr, 0, nullptr);
    for (const auto& u_ptr : profiles) {
        list<Post*>* posts = u_ptr->getPosts();
        for (Post* p : *posts) {
            string sql = "INSERT INTO POSTS VALUES ('" + 
                         p->getText() + "', " + 
                         to_string((long long)p->getDate()) + ", " + 
                         to_string(u_ptr->getId()) + ", " +
                         "'" + p->getType() + "', " + 
                         "'" + p->getMediaPath() + "');";
            sqlite3_exec(db, sql.c_str(), nullptr, 0, nullptr);
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

            // Lógica unificada: Só existe Page ou User
            if (type == "Page") {
                p = new Page(name, nullptr, password, icon, bio, subtitle, startDate); 
                if (ownerId != -1) pendingOwners[id] = ownerId;
            }
            else {
                // Aqui criamos o User, passando o status de verificação e o email
                // IMPORTANTE: Seu construtor de User em User.cpp deve aceitar esses parametros nessa ordem
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
            // O Dono agora é do tipo User (que pode ser verificado ou não)
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
    if (sqlite3_prepare_v2(db, "SELECT * FROM POSTS", -1, &stmtPost, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmtPost) == SQLITE_ROW) {
            string txt = string(reinterpret_cast<const char*>(sqlite3_column_text(stmtPost, 0)));
            time_t date = (time_t)sqlite3_column_int64(stmtPost, 1);
            int authorId = sqlite3_column_int(stmtPost, 2);
            
            string type = "TEXT";
            if (sqlite3_column_text(stmtPost, 3)) 
                type = string(reinterpret_cast<const char*>(sqlite3_column_text(stmtPost, 3)));

            string mediaPath = "";
            if (sqlite3_column_text(stmtPost, 4))
                mediaPath = string(reinterpret_cast<const char*>(sqlite3_column_text(stmtPost, 4)));

            try {
                Profile* author = sn->getProfile(authorId);
                if (author) {
                    if (type == "IMAGE") {
                        author->addPost(new ImagePost(txt, date, author, mediaPath));
                    } else {
                        author->addPost(new Post(txt, date, author));
                    }
                }
            } catch (...) {}
        }
    }
    sqlite3_finalize(stmtPost);
    
    cout << "Data loaded successfully!" << endl;
}
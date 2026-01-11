#include "NetworkStorage.h"
#include <iostream>
#include <map>

using namespace std;

NetworkStorage::NetworkStorage(string dbFileName) {
    this->dbFileName = dbFileName;
    this->db = nullptr;

    int exit = sqlite3_open(dbFileName.c_str(), &db);
    if (exit != SQLITE_OK) cerr << "Error opening DB" << endl;
    else cout << "Database opened successfully!" << endl;

    // 1. Tabela PROFILES atualizada (Agora tem OWNER_ID)
    string sql = "CREATE TABLE IF NOT EXISTS PROFILES("
                 "ID INT PRIMARY KEY NOT NULL, "
                 "NAME TEXT NOT NULL, "
                 "TYPE TEXT NOT NULL, "
                 "EMAIL TEXT, "     
                 "OWNER_ID INT, "
                 "PASSWORD TEXT );";
    executeSQL(sql);

    // 2. Conexões (Igual)
    executeSQL("CREATE TABLE IF NOT EXISTS CONNECTIONS(ID1 INT NOT NULL, ID2 INT NOT NULL);");
    
    // 3. Posts (Igual)
    executeSQL("CREATE TABLE IF NOT EXISTS POSTS(TEXT TEXT NOT NULL, DATE INT NOT NULL, AUTHOR_ID INT NOT NULL);");
}

NetworkStorage::~NetworkStorage() {
    sqlite3_close(db);
    cout << "Database connection closed." << endl;
}

void NetworkStorage::executeSQL(string sql) {
    char* zErrMsg = 0;
    
    // sqlite3_exec executa o comando SQL direto
    int rc = sqlite3_exec(db, sql.c_str(), nullptr, 0, &zErrMsg);

    if (rc != SQLITE_OK) {
        cerr << "SQL Error: " << zErrMsg << endl;
        sqlite3_free(zErrMsg);
    } else {
        cout << "SQL executed successfully" << endl; 
    }
}

void NetworkStorage::save(SocialNetwork* sn) {
    cout << "DEBUG: Save iniciado." << endl;
    char* zErrMsg = 0;
    
    // --- PARTE 1: PERFIS ---
    sqlite3_exec(db, "DELETE FROM PROFILES;", nullptr, 0, nullptr);
    
    const auto& profiles = sn->getProfiles();

    for (const auto& u_ptr : profiles) {
        // Logica para descobrir o ID do dono (se for Pagina)
        int ownerId = -1; // -1 significa "Sem dono"
        Page* pagePtr = dynamic_cast<Page*>(u_ptr.get());
        
        if (pagePtr != nullptr && pagePtr->getOwner() != nullptr) {
            ownerId = pagePtr->getOwner()->getId();
        }

        // Montando o SQL com 5 colunas
        string sql = "INSERT INTO PROFILES VALUES (";
        sql += to_string(u_ptr->getId()) + ", ";
        sql += "'" + u_ptr->getName() + "', ";
        sql += "'" + u_ptr->getRole() + "', ";
        
        // Email
        VerifiedUser* vPtr = dynamic_cast<VerifiedUser*>(u_ptr.get());
        if (vPtr) sql += "'" + vPtr->getEmail() + "', ";
        else sql += "'', "; // Email vazio
        
        // Owner ID
        sql += to_string(ownerId) + ");"; 

        // Senha
        sql += "'" + u_ptr->getPassword() + "');";

        sqlite3_exec(db, sql.c_str(), nullptr, 0, nullptr);
    }

    // --- PARTE 2: CONEXOES (Mantenha o codigo anterior aqui...) ---
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

    // --- PARTE 3: POSTS (Mantenha o codigo anterior aqui...) ---
    sqlite3_exec(db, "DELETE FROM POSTS;", nullptr, 0, nullptr);
    for (const auto& u_ptr : profiles) {
        list<Post*>* posts = u_ptr->getPosts();
        for (Post* p : *posts) {
            string sql = "INSERT INTO POSTS VALUES ('" + 
                         p->getText() + "', " + 
                         to_string((long long)p->getDate()) + ", " + 
                         to_string(u_ptr->getId()) + ");";
            sqlite3_exec(db, sql.c_str(), nullptr, 0, nullptr);
        }
    }
    cout << "DEBUG: Save concluido." << endl;
}

void NetworkStorage::load(SocialNetwork* sn) {
    cout << "DEBUG: Loading..." << endl;
    
    // Mapa temporario: ID da Pagina -> ID do Dono
    map<int, int> pendingOwners; 

    // --- FASE 1: PERFIS ---
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, "SELECT * FROM PROFILES", -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            string name = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
            string type = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)));
            
            // Email (coluna 3)
            string email = "";
            const unsigned char* em = sqlite3_column_text(stmt, 3);
            if(em) email = string(reinterpret_cast<const char*>(em));

            // Owner ID (coluna 4)
            int ownerId = sqlite3_column_int(stmt, 4);

            string password = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5)));

            Profile* p = nullptr;
            if (type == "Verified User") p = new VerifiedUser(name, email, password);
            else if (type == "Page") {
                p = new Page(name, nullptr, password); // Cria sem dono por enquanto
                if (ownerId != -1) pendingOwners[id] = ownerId; // Anota pra resolver depois
            }
            else p = new User(name, password);

            p->setId(id);
            sn->add(p);
        }
    }
    sqlite3_finalize(stmt);

    // --- FASE EXTRA: RECONECTAR DONOS DAS PAGINAS ---
    for (auto const& [pageId, ownerId] : pendingOwners) {
        try {
            // Pegamos a pagina e o dono da lista de perfis carregados
            Page* pg = dynamic_cast<Page*>(sn->getProfile(pageId));
            VerifiedUser* vu = dynamic_cast<VerifiedUser*>(sn->getProfile(ownerId));
            
            if (pg && vu) {
                pg->setOwner(vu);
                // cout << "DEBUG: Dono restaurado para a pagina " << pg->getName() << endl;
            }
        } catch (...) { cout << "Erro ao restaurar dono." << endl; }
    }

    // --- FASE 2: CONEXOES (Seu codigo anterior de conexoes vai aqui...) ---
    // (Lembre-se de copiar o bloco de connections e posts que ja fizemos!)
    // Vou resumir aqui para nao ficar gigante, mas mantenha o que voce ja tinha:
    
    // Load Connections...
    sqlite3_stmt* stmtRel;
    if (sqlite3_prepare_v2(db, "SELECT * FROM CONNECTIONS", -1, &stmtRel, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmtRel) == SQLITE_ROW) {
            try {
                sn->getProfile(sqlite3_column_int(stmtRel, 0))->addContact(sn->getProfile(sqlite3_column_int(stmtRel, 1)));
            } catch (...) {}
        }
    }
    sqlite3_finalize(stmtRel);

    // Load Posts...
    sqlite3_stmt* stmtPost;
    if (sqlite3_prepare_v2(db, "SELECT * FROM POSTS", -1, &stmtPost, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmtPost) == SQLITE_ROW) {
            try {
                 string txt = string(reinterpret_cast<const char*>(sqlite3_column_text(stmtPost, 0)));
                 time_t date = (time_t)sqlite3_column_int64(stmtPost, 1);
                 int authorId = sqlite3_column_int(stmtPost, 2);
                 Profile* author = sn->getProfile(authorId);
                 author->addPost(new Post(txt, date, author));
            } catch (...) {}
        }
    }
    sqlite3_finalize(stmtPost);
    
    cout << "Data loaded successfully!" << endl;
}
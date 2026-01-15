#include "API/Router.h"
#include "Core/Database.h"
#include "Core/Translation.h"
#include "Core/Utils.h"
#include <iostream>

int main() {

    Core::Utils::loadEnv(".env");
    
    // 1. Inicializa o Banco de Dados
    auto* db = Core::Database::getInstance();

    // --- CRIAÇÃO DE TABELAS (SCHEMA) ---
    
    // USERS
    db->execute("CREATE TABLE IF NOT EXISTS users ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "username TEXT, "
                    "email TEXT UNIQUE, "
                    "password_hash TEXT, " 
                    "bio TEXT, "
                    "birth_date TEXT, "
                    "is_private INTEGER DEFAULT 0, "  // 0 Publico, 1 Privado
                    "language TEXT DEFAULT 'en_US', "
                    "creation_date TEXT);"); 

    // POSTS
    db->execute("CREATE TABLE IF NOT EXISTS posts ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT, "
            "author_id INTEGER, "
            "community_id INTEGER DEFAULT NULL, " 
            "content TEXT, "
            "tags TEXT, "
            "creation_date TEXT, "
            "FOREIGN KEY(author_id) REFERENCES users(id), "
            "FOREIGN KEY(community_id) REFERENCES communities(id));");

    // FRIENDSHIPS (Com Status)
    db->execute("CREATE TABLE IF NOT EXISTS friendships (user_id_1 INTEGER, user_id_2 INTEGER, status INTEGER DEFAULT 0, since_date TEXT, PRIMARY KEY (user_id_1, user_id_2), FOREIGN KEY(user_id_1) REFERENCES users(id), FOREIGN KEY(user_id_2) REFERENCES users(id));");

    // COMMENTS (Com Parent_ID para Threads)
    db->execute("CREATE TABLE IF NOT EXISTS comments ("
                "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                "post_id INTEGER, "
                "author_id INTEGER, "
                "parent_id INTEGER DEFAULT NULL, "
                "content TEXT, "
                "creation_date TEXT, "
                "FOREIGN KEY(post_id) REFERENCES posts(id), "
                "FOREIGN KEY(author_id) REFERENCES users(id), "
                "FOREIGN KEY(parent_id) REFERENCES comments(id));");

    // LIKES 
    db->execute("CREATE TABLE IF NOT EXISTS likes ("
                "post_id INTEGER, "
                "user_id INTEGER, "
                "date TEXT, "
                "PRIMARY KEY (post_id, user_id), " 
                "FOREIGN KEY(post_id) REFERENCES posts(id), "
                "FOREIGN KEY(user_id) REFERENCES users(id));");

    // TABELA DE LOGS DO SISTEMA (AUDIT)
    // action: "BIO_UPDATE", "POST_CREATE", "LOGIN", "PASS_CHANGE"
    db->execute("CREATE TABLE IF NOT EXISTS system_logs ("
                "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                "user_id INTEGER, "
                "action TEXT, "
                "details TEXT, " 
                "ip_address TEXT, " // soon feature
                "date TEXT, "
                "FOREIGN KEY(user_id) REFERENCES users(id));");

    // NOTIFICAÇÕES
    db->execute("CREATE TABLE IF NOT EXISTS notifications ("
                "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                "user_id INTEGER, "     
                "sender_id INTEGER, "    
                "type INTEGER, "         // 1=Req, 2=Like, 3=Comment
                "reference_id INTEGER, " 
                "content TEXT, "         // Chave de tradução
                "is_read INTEGER DEFAULT 0, "
                "creation_date TEXT, "
                "FOREIGN KEY(user_id) REFERENCES users(id));");

    // COMUNIDADES
    db->execute("CREATE TABLE IF NOT EXISTS communities ("
                "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                "name TEXT, "
                "description TEXT, "
                "owner_id INTEGER, "
                "is_private INTEGER DEFAULT 0, "
                "creation_date TEXT);");
    
    // Membros da comunidade
    db->execute("CREATE TABLE IF NOT EXISTS community_members ("
                "community_id INTEGER, "
                "user_id INTEGER, "
                "role INTEGER, "
                "join_date TEXT, "
                "PRIMARY KEY (community_id, user_id));");

    // Tabela para solicitações de entrada (Join Requests)
    db->execute("CREATE TABLE IF NOT EXISTS community_requests ("
                "community_id INTEGER, "
                "user_id INTEGER, "
                "request_date TEXT, "
                "status INTEGER DEFAULT 0, "
                "PRIMARY KEY (community_id, user_id));");

    // TABELA DE INTERESSES (Cérebro do Algoritmo)
    db->execute("CREATE TABLE IF NOT EXISTS user_interests ("
                "user_id INTEGER, "
                "tag TEXT, "
                "weight INTEGER DEFAULT 0, "
                "PRIMARY KEY (user_id, tag), "
                "FOREIGN KEY(user_id) REFERENCES users(id));");

    // 2. Configura Tradução
    Core::Translation::getInstance()->setLanguage(Core::Language::PT_BR); // Padrão

    // 3. Inicia Servidor
    crow::SimpleApp app;
    API::Router::setupRoutes(app);

    std::cout << ">> Social Engine Backend Operational on Port 8085..." << std::endl;
    app.port(8085).multithreaded().run();

    return 0;
}
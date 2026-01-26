#include "API/Router.h"
#include "Core/Database.h"
#include "Core/Translation.h"
#include "Core/Location.h"
#include "Core/Utils.h"
#include <iostream>

int main() {
    Core::Utils::loadEnv(".env");

    // 1. Inicializa o Banco de Dados
    auto* db = Core::Database::getInstance();

    db->execute("PRAGMA foreign_keys = ON;");
    
    // USERS (Com Geolocalização)
    db->execute("CREATE TABLE IF NOT EXISTS users ("
                "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                "username TEXT, "
                "email TEXT UNIQUE, "
                "password_hash TEXT, " 
                "bio TEXT, "
                "birth_date TEXT, "
                "city TEXT, "       
                "state TEXT, "     
                "avatar_url TEXT DEFAULT '', " 
                "cover_url TEXT DEFAULT '', " 
                "is_private INTEGER DEFAULT 0, "
                "is_verified INTEGER DEFAULT 0, "
                "role INTEGER DEFAULT 0, "    
                "language TEXT DEFAULT 'pt_BR', "
                "creation_date TEXT);");

    // POSTS (Com Mídia)
    db->execute("CREATE TABLE IF NOT EXISTS posts ("
                "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                "author_id INTEGER, "
                "community_id INTEGER DEFAULT NULL, " 
                "content TEXT, "
                "tags TEXT, "
                "media_url TEXT DEFAULT '', "   
                "media_type TEXT DEFAULT '', " 
                "creation_date TEXT, "
                "FOREIGN KEY(author_id) REFERENCES users(id), "
                "FOREIGN KEY(community_id) REFERENCES communities(id));");

    // FRIENDSHIPS
    db->execute("CREATE TABLE IF NOT EXISTS friendships (user_id_1 INTEGER, user_id_2 INTEGER, status INTEGER DEFAULT 0, since_date TEXT, PRIMARY KEY (user_id_1, user_id_2), FOREIGN KEY(user_id_1) REFERENCES users(id), FOREIGN KEY(user_id_2) REFERENCES users(id));");

    // COMMENTS
    // 1. Tabela de Comentários (Agora com Mídia e Pai)
    db->execute("CREATE TABLE IF NOT EXISTS comments ("
                "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                "post_id INTEGER, "
                "author_id INTEGER, "
                "parent_id INTEGER DEFAULT -1, " 
                "content TEXT, "
                "media_url TEXT DEFAULT '', "   
                "media_type TEXT DEFAULT '', "   
                "creation_date TEXT, "
                "FOREIGN KEY(post_id) REFERENCES posts(id), "
                "FOREIGN KEY(author_id) REFERENCES users(id));");

    // 2. Tabela de Likes Específica para Comentários
    // (Separada de post_likes para não bagunçar a lógica)
    db->execute("CREATE TABLE IF NOT EXISTS comment_likes ("
                "user_id INTEGER, "
                "comment_id INTEGER, "
                "date TEXT, "
                "PRIMARY KEY (user_id, comment_id), "
                "FOREIGN KEY(user_id) REFERENCES users(id), "
                "FOREIGN KEY(comment_id) REFERENCES comments(id));");

    // LIKES 
    db->execute("CREATE TABLE IF NOT EXISTS likes ("
                "post_id INTEGER, "
                "user_id INTEGER, "
                "date TEXT, "
                "PRIMARY KEY (post_id, user_id), " 
                "FOREIGN KEY(post_id) REFERENCES posts(id), "
                "FOREIGN KEY(user_id) REFERENCES users(id));");

    // LOGS
    db->execute("CREATE TABLE IF NOT EXISTS system_logs ("
                "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                "user_id INTEGER, "
                "action TEXT, "
                "details TEXT, " 
                "ip_address TEXT, "
                "date TEXT, "
                "FOREIGN KEY(user_id) REFERENCES users(id));");

    // NOTIFICATIONS
    // Se o post for deletado, a notificação some sozinha (ON DELETE CASCADE)
    db->execute("CREATE TABLE IF NOT EXISTS notifications ("
                "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                "user_id INTEGER, "
                "sender_id INTEGER, "
                "type INTEGER, "        
                "post_id INTEGER DEFAULT -1, "      
                "community_id INTEGER DEFAULT -1, "  
                "content TEXT, "
                "is_read INTEGER DEFAULT 0, "
                "created_at TEXT, "     
                "FOREIGN KEY(user_id) REFERENCES users(id) ON DELETE CASCADE, "
                "FOREIGN KEY(sender_id) REFERENCES users(id) ON DELETE CASCADE, "
                "FOREIGN KEY(post_id) REFERENCES posts(id) ON DELETE CASCADE);");

    // COMUNIDADES (Com Geolocalização)
    db->execute("CREATE TABLE IF NOT EXISTS communities ("
                "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                "owner_id INTEGER, "
                "name TEXT, "
                "description TEXT, "
                "cover_url TEXT DEFAULT '', " 
                "city TEXT, "       
                "state TEXT, "      
                "is_private INTEGER DEFAULT 0, "
                "creation_date TEXT, "
                "FOREIGN KEY(owner_id) REFERENCES users(id));");
    
    // MEMBROS COMUNIDADE
    db->execute("CREATE TABLE IF NOT EXISTS community_members ("
                "community_id INTEGER, "
                "user_id INTEGER, "
                "role INTEGER, "
                "join_date TEXT, "
                "PRIMARY KEY (community_id, user_id));");

    // PEDIDOS COMUNIDADE
    db->execute("CREATE TABLE IF NOT EXISTS community_requests ("
                "community_id INTEGER, "
                "user_id INTEGER, "
                "request_date TEXT, "
                "status INTEGER DEFAULT 0, "
                "PRIMARY KEY (community_id, user_id));");

    // INTERESSES
    db->execute("CREATE TABLE IF NOT EXISTS user_interests ("
                "user_id INTEGER, "
                "tag TEXT, "
                "weight INTEGER DEFAULT 0, "
                "PRIMARY KEY (user_id, tag), "
                "FOREIGN KEY(user_id) REFERENCES users(id));");

    // REPORTS (Com Categoria e Tipo Genérico)
    db->execute("CREATE TABLE IF NOT EXISTS reports ("
                "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                "reporter_id INTEGER, "
                "target_id INTEGER, "
                "target_type INTEGER, "
                "category INTEGER DEFAULT 5, "
                "reason TEXT, "
                "status INTEGER DEFAULT 0, "
                "creation_date TEXT, "
                "FOREIGN KEY(reporter_id) REFERENCES users(id));");
    
    // --- NOVO: TABELAS GEOGRÁFICAS ---
    db->execute("CREATE TABLE IF NOT EXISTS states ("
                "code TEXT PRIMARY KEY, "   
                "name TEXT, "            
                "country TEXT);");        

    db->execute("CREATE TABLE IF NOT EXISTS cities ("
                "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                "name TEXT, "
                "state_code TEXT, "
                "FOREIGN KEY(state_code) REFERENCES states(code));");

    // 2. Configura Tradução e Localizações
    Core::Translation::getInstance()->setLanguage(Core::Language::PT_BR);
    Core::Location::seed();

    // 3. Inicia Servidor (TIPO CORRETO AGORA: SimpleApp)
    crow::SimpleApp app;
    
    // Inicia Rotas
    API::Router::setupRoutes(app);

    // --- ROTA DA HOME (SERVE O FRONTEND) ---
    // ROTA PRINCIPAL
    CROW_ROUTE(app, "/")
    ([](){
        std::vector<std::string> paths = {
            "index.html", 
            "frontend/index.html",
            "../frontend/index.html",
            "../../frontend/index.html"
        };

        std::ifstream in;
        std::string foundPath = "";

        for (const auto& path : paths) {
            // DEBUG: Mostra onde está procurando
            std::cout << ">> Tentando abrir: " << path << "... "; 
            
            in.open(path);
            if (in.is_open()) {
                std::cout << "ENCONTRADO!" << std::endl; // <--- Dedura
                foundPath = path;
                break;
            } else {
                std::cout << "X" << std::endl;
            }
            in.clear();
        }

        if (in.is_open()) {
            std::ostringstream contents;
            contents << in.rdbuf();
            in.close();
            
            std::string html = contents.str();
            
            // DEBUG: Verifica se o arquivo está vazio
            if (html.empty()) {
                 std::cout << ">> ALERTA: O arquivo " << foundPath << " esta VAZIO (0 bytes)!" << std::endl;
                 return crow::response(500, "ERRO: O arquivo index.html existe mas esta vazio.");
            }

            return crow::response(html);
        } else {
            return crow::response(404, "ERRO 404: Index.html nao encontrado.");
        }
    });

    // -----------------------------------------------------------------------
    // CORS MANUAL: Rotas OPTIONS (Pre-flight)
    // Precisamos cobrir todos os níveis de profundidade da API
    // -----------------------------------------------------------------------
    auto handleOptions = [](const crow::request&, crow::response& res){
        res.add_header("Access-Control-Allow-Origin", "*");
        res.add_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res.add_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
        res.code = 204; 
        res.end();
    };

    // Nível 1: /health
    CROW_ROUTE(app, "/<path>").methods(crow::HTTPMethod::Options)(
        [](const crow::request&, crow::response& res, std::string){ 
            res.add_header("Access-Control-Allow-Origin", "*");
            res.add_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
            res.add_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
            res.code = 204; res.end(); 
        }
    );
    
    // Nível 2: /api/login, /api/signup, /api/states
    CROW_ROUTE(app, "/api/<path>").methods(crow::HTTPMethod::Options)(
        [](const crow::request&, crow::response& res, std::string){ 
            res.add_header("Access-Control-Allow-Origin", "*");
            res.add_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
            res.add_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
            res.code = 204; res.end(); 
        }
    );

    // Nível 3: /api/friends/request, /api/communities/leave
    CROW_ROUTE(app, "/api/<path>/<path>").methods(crow::HTTPMethod::Options)(
        [](const crow::request&, crow::response& res, std::string, std::string){ 
            res.add_header("Access-Control-Allow-Origin", "*");
            res.add_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
            res.add_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
            res.code = 204; res.end(); 
        }
    );

    // -----------------------------------------------------------------------
    // ROTA DE ARQUIVOS ESTÁTICOS (UPLOADS) + CORS
    // -----------------------------------------------------------------------
    CROW_ROUTE(app, "/uploads/<string>")
    ([](const crow::request&, crow::response& res, std::string filename){
        std::ifstream file("uploads/" + filename, std::ios::binary);
        if (file.is_open()) {
            std::ostringstream oss;
            oss << file.rdbuf();
            res.write(oss.str());
            
            // Tipos MIME
            if (filename.find(".jpg") != std::string::npos) res.add_header("Content-Type", "image/jpeg");
            else if (filename.find(".png") != std::string::npos) res.add_header("Content-Type", "image/png");
            else if (filename.find(".mp4") != std::string::npos) res.add_header("Content-Type", "video/mp4");
            
            // !!! CORS MANUAL !!!
            res.add_header("Access-Control-Allow-Origin", "*"); 
            
            res.end();
        } else {
            res.code = 404;
            res.write("Not Found");
            res.end();
        }
    });

    // Cria pasta
    system("mkdir uploads 2> NUL");

    std::cout << ">> Social Engine Backend Operational on Port 8085..." << std::endl;
    app.port(8085).multithreaded().run();

    return 0;
}
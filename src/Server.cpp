#define CROW_MAIN
#define ASIO_STANDALONE 
#include "crow_all.h"
#include "SocialNetwork.h"
#include "NetworkStorage.h"
#include "User.h"
#include <iostream>

int main() {
    // =================================================================================
    // 1. INICIALIZAÇÃO DO MOTOR E BANCO DE DADOS
    // =================================================================================
    std::cout << ">> Inicializando o motor da Rede Social...\n";
    SocialNetwork sn;
    NetworkStorage storage("rede_social.db");
    
    try {
        storage.load(&sn);
        std::cout << ">> Banco de dados carregado com sucesso!\n";
    } catch (...) {
        std::cout << ">> Aviso: Iniciando banco novo ou erro na leitura.\n";
    }

    // =================================================================================
    // 2. CONFIGURAÇÃO DAS ROTAS (ENDPOINTS)
    // =================================================================================
    crow::App<crow::CORSHandler> app;

    auto& cors = app.get_middleware<crow::CORSHandler>();
    cors.global()
        .headers("X-Custom-Header", "Upgrade-Insecure-Requests")
        .methods("POST"_method, "GET"_method)
        .origin("*");

    // -------------------------------------------------------------------------
    // GRUPO: ROTAS DE LEITURA (GET) - Apenas consultam dados
    // -------------------------------------------------------------------------

    // ROTA RAIZ: Página de boas-vindas
    CROW_ROUTE(app, "/")
    ([](){
        return "<h1>Jam Guild API</h1><p>Backend C++ rodando! Use /api/signup para criar contas.</p>";
    });

    // ROTA STATUS: Verifica saúde do servidor
    CROW_ROUTE(app, "/api/status")
    ([&sn](){
        crow::json::wvalue x;
        x["status"] = "Online";
        x["server"] = "Jam Guild C++ Backend";
        x["profiles_count"] = sn.getProfilesAmount();
        return x;
    });

    // ROTA PERFIL: Busca dados de um usuário pelo ID
    CROW_ROUTE(app, "/api/profile/<int>")
    ([&sn](int id){
        try {
            Profile* p = sn.getProfile(id);
            crow::json::wvalue json;
            json["id"] = p->getId();
            json["name"] = p->getName();
            json["bio"] = p->getBio();
            json["type"] = p->getRole();
            
            User* u = dynamic_cast<User*>(p);
            if (u) json["verified"] = u->isVerified();

            return crow::response(json);
        } catch (...) {
            return crow::response(404, "{\"error\": \"User not found\"}");
        }
    });

    // ROTA TIMELINE: Busca os posts de um usuário
    CROW_ROUTE(app, "/api/timeline/<int>")
    ([&sn](int userId){
        try {
            Profile* p = sn.getProfile(userId);
            User* u = dynamic_cast<User*>(p);
            if (!u) return crow::response(400, "Apenas usuarios tem timeline");

            std::vector<Post*> timeline = sn.getTimeline(u);
            crow::json::wvalue x;
            
            int i = 0;
            for (auto* post : timeline) {
                x[i]["author"] = post->getOwner()->getName();
                x[i]["text"] = post->getText();
                x[i]["date"] = post->getFormattedDate();
                x[i]["type"] = post->getType();
                i++;
            }
            return crow::response(x);
        } catch (...) {
            return crow::response(404, "Usuario nao encontrado");
        }
    });

    // -------------------------------------------------------------------------
    // GRUPO: ROTAS DE ESCRITA (POST) - Modificam o banco de dados
    // -------------------------------------------------------------------------

    // ROTA CADASTRO: Cria um novo usuário
    // JSON esperado: { "username": "Nome", "type": "USER" }
    CROW_ROUTE(app, "/api/signup").methods(crow::HTTPMethod::Post)
    ([&sn, &storage](const crow::request& req){ 
        auto x = crow::json::load(req.body);
        if (!x) return crow::response(400, "JSON invalido");

        std::string username = x["username"].s();
        std::string password = x["password"].s();

        if (username.empty() || password.empty()) 
            return crow::response(400, "Username e Password sao obrigatorios");

        try {
            Profile* newP = sn.signUp(username, password);
            
            storage.saveUser(newP); 

            crow::json::wvalue resp;
            resp["message"] = "Usuario criado com sucesso!";
            resp["id"] = newP->getId();
            return crow::response(201, resp);
        } catch (const std::exception& e) {
            return crow::response(500, e.what());
        }
    });

    // ROTA LOGIN (Otimizada com Map)
    // JSON esperado: { "username": "Jeff", "password": "123" }
    CROW_ROUTE(app, "/api/signin").methods(crow::HTTPMethod::Post)
    ([&sn](const crow::request& req){
        auto x = crow::json::load(req.body);
        if (!x) return crow::response(400, "JSON invalido");

        std::string username = x["username"].s();
        std::string password = x["password"].s();

        // 1. Busca Instantânea pelo Mapa
        int foundId = sn.getIdByUsername(username);

        if (foundId == -1) return crow::response(401, "Usuario nao encontrado");

        // 2. Tenta Logar
        Profile* loggedUser = sn.signIn(foundId, password);

        if (loggedUser) {
            crow::json::wvalue resp;
            resp["message"] = "Login realizado!";
            resp["id"] = loggedUser->getId();
            resp["name"] = loggedUser->getName();
            resp["icon"] = loggedUser->getIconPath();
            resp["is_verified"] = (loggedUser->getRole() == "Verified User");
            
            return crow::response(200, resp);
        } else {
            return crow::response(401, "Senha incorreta");
        }
    });

    // ROTA CRIAR PÁGINA (Exige dono verificado)
    // JSON esperado: { "name": "Minha Empresa", "owner_id": 1 }
    CROW_ROUTE(app, "/api/pages").methods(crow::HTTPMethod::Post)
    ([&sn, &storage](const crow::request& req){
        auto x = crow::json::load(req.body);
        if (!x) return crow::response(400, "JSON invalido");

        std::string pageName = x["name"].s();
        int ownerId = x["owner_id"].i();

        if (pageName.empty()) return crow::response(400, "Nome da pagina obrigatorio");

        try {
            // 1. Busca o dono pelo ID
            Profile* p = sn.getProfile(ownerId);
            User* owner = dynamic_cast<User*>(p);

            if (!owner) {
                return crow::response(400, "ID do proprietario invalido (nao e um usuario).");
            }

            // 2. Tenta criar a página (A lógica de 'isVerified' roda aqui dentro)
            Profile* newPage = sn.createPage(owner, pageName);

            // 3. Salva no banco
            storage.saveUser(newPage);

            crow::json::wvalue resp;
            resp["message"] = "Pagina criada com sucesso!";
            resp["id"] = newPage->getId();
            return crow::response(201, resp);

        } catch (const std::exception& e) {
            // Aqui pegamos o erro "Apenas usuarios verificados..."
            return crow::response(403, e.what()); 
        }
    });

    // ROTA CRIAR POST
    // JSON: { "userId": 1, "text": "Ola mundo!", "type": "TEXT" }
    CROW_ROUTE(app, "/api/posts").methods(crow::HTTPMethod::Post)
    ([&sn, &storage](const crow::request& req){
        auto x = crow::json::load(req.body);
        if (!x) return crow::response(400, "JSON invalido");

        int userId = x["userId"].i();
        std::string text = x["text"].s();
        
        // Validação básica
        if (text.empty()) return crow::response(400, "Texto nao pode ser vazio");

        try {
            Profile* author = sn.getProfile(userId);
            
            // Cria o post (na memória)
            Post* newPost = new Post(text, author);
            author->addPost(newPost); // Vincula ao autor

            // Salva TUDO no banco (para garantir que o post fique lá)
            // Obs: Idealmente teríamos storage.savePost(), mas save() resolve por hora
            storage.save(&sn); 

            return crow::response(201, "Post criado!");
        } catch (...) {
            return crow::response(404, "Usuario nao encontrado");
        }
    });


    // =================================================================================
    // >>> ESPAÇO RESERVADO PARA FUTURAS ROTAS <<<
    // Coloque aqui:
    // - POST /api/post (Criar postagem)
    // - POST /api/connect (Seguir alguém)
    // =================================================================================

    // ... NOVAS ROTAS AQUI ...


    // =================================================================================
    // 3. START DO SERVIDOR
    // =================================================================================
    std::cout << ">> Servidor rodando em http://127.0.0.1:8085\n";
    // Bind em 0.0.0.0 para evitar conflitos de IP no Windows
    app.bindaddr("0.0.0.0").port(8085).multithreaded().run();
    
    return 0;
}
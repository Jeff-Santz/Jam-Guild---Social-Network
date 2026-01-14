#define CROW_MAIN
#define ASIO_STANDALONE 
#include "crow_all.h"
#include "SocialNetwork.h"
#include "NetworkStorage.h"
#include "User.h"
#include "ContactHandlers.h"
#include <iostream>
#include <iterator>

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

    // ROTA: EDITAR PERFIL (PUT)
    // JSON: { "bio": "Nova bio", "icon": "caminho/foto.png", "gender": "M" }
    // Obs: Campos são opcionais. Se não mandar, mantém o antigo.
    CROW_ROUTE(app, "/api/profile/<int>").methods(crow::HTTPMethod::Put)
    ([&sn, &storage](const crow::request& req, int profileId){
        auto x = crow::json::load(req.body);
        if (!x) return crow::response(400, "JSON invalido");

        try {
            Profile* p = sn.getProfile(profileId);

            if (x.has("bio")) {
                p->setBio(x["bio"].s()); 
            }
            if (x.has("icon")) {
                p->setIconPath(x["icon"].s()); 
            }

            User* u = dynamic_cast<User*>(p);
            if (u) {
                if (x.has("gender")) {
                    p->setSubtitle(x["gender"].s()); 
                }
            }

            // 3. Salva
            storage.save(&sn);

            return crow::response(200, "Perfil atualizado!");

        } catch (...) {
            return crow::response(404, "Perfil nao encontrado");
        }
    });

    // ROTA: VERIFICAR USUÁRIO (Admin/Debug)
    // JSON: { "userId": 1, "email": "jeff@usp.br" }
    CROW_ROUTE(app, "/api/profile/verify").methods(crow::HTTPMethod::Post)
    ([&sn, &storage](const crow::request& req){
        auto x = crow::json::load(req.body);
        if (!x) return crow::response(400, "JSON invalido");

        int userId = x["userId"].i();
        
        if (!x.has("email")) return crow::response(400, "Email obrigatorio para verificacao.");
        std::string email = x["email"].s();

        try {
            Profile* p = sn.getProfile(userId);
            User* u = dynamic_cast<User*>(p);

            if (u) {
                u->verify(email); 
                
                storage.save(&sn);
                return crow::response(200, "Usuario verificado com sucesso!");
            } else {
                return crow::response(400, "Apenas usuarios (humanos) podem ser verificados.");
            }
        } catch (...) {
            return crow::response(404, "Usuario nao encontrado");
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
            Profile* p = sn.getProfile(ownerId);
            User* owner = dynamic_cast<User*>(p);

            if (!owner) {
                return crow::response(400, "ID do proprietario invalido (nao e um usuario).");
            }
            Profile* newPage = sn.createPage(owner, pageName);
            storage.saveUser(newPage);
            crow::json::wvalue resp;
            resp["message"] = "Pagina criada com sucesso!";
            resp["id"] = newPage->getId();
            return crow::response(201, resp);

        } catch (const std::exception& e) {
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
        
        if (text.empty()) return crow::response(400, "Texto nao pode ser vazio");
        try {
            Profile* author = sn.getProfile(userId);
            
            Post* newPost = new Post(text, author);
            author->addPost(newPost); // Vincula ao autor
            storage.save(&sn); 

            return crow::response(201, "Post criado!");
        } catch (...) {
            return crow::response(404, "Usuario nao encontrado");
        }
    });

    // ROTA BUSCA DE PERFIS
    // JSON: { "query": "Jeff" }
    CROW_ROUTE(app, "/api/search").methods(crow::HTTPMethod::Post)
    ([&sn](const crow::request& req){
        auto x = crow::json::load(req.body);
        if (!x) return crow::response(400, "JSON invalido");

        std::string query = x["query"].s();
        
        std::vector<Profile*> results = sn.searchProfiles(query);

        crow::json::wvalue jsonResp;
        int i = 0;
        for(auto* p : results) {
            jsonResp[i]["id"] = p->getId();
            jsonResp[i]["name"] = p->getName();
            jsonResp[i]["icon"] = p->getIconPath();
            jsonResp[i]["type"] = p->getRole();
            jsonResp[i]["is_verified"] = (p->getRole() == "Verified User");
            i++;
        }
        return crow::response(200, jsonResp);
    });

    // =================================================================================
    // >>> ESPAÇO RESERVADO PARA FUTURAS ROTAS <<<
    // Coloque aqui:
    // - POST /api/post (Criar postagem)
    // - POST /api/connect (Seguir alguém)
    // =================================================================================

    // ... NOVAS ROTAS AQUI ...

    // -------------------------------------------------------------------------
    // GRUPO: AMIZADES E INTERAÇÕES
    // -------------------------------------------------------------------------

    // 1. ENVIAR PEDIDO DE AMIZADE
    // JSON: { "userId": 1, "targetName": "Maria" }
    CROW_ROUTE(app, "/api/friends/request").methods(crow::HTTPMethod::Post)
    ([&sn, &storage](const crow::request& req){
        auto x = crow::json::load(req.body);
        int userId = x["userId"].i();
        std::string targetName = x["targetName"].s();

        try {
            Profile* sender = sn.getProfile(userId);
            
            int targetId = sn.getIdByUsername(targetName);
            if (targetId == -1) return crow::response(404, "Usuario nao encontrado");
            Profile* target = sn.getProfile(targetId);
            ContactHandlers::coreSendFriendRequest(sender, target, sn, storage);

            return crow::response(200, "Pedido enviado com sucesso!");
        } catch (const std::exception& e) {
            return crow::response(400, e.what());
        }
    });

    // 2. RESPONDER PEDIDO (ACEITAR / RECUSAR)
    // JSON: { "userId": 1, "targetId": 5, "action": "ACCEPT" }
    CROW_ROUTE(app, "/api/friends/respond").methods(crow::HTTPMethod::Post)
    ([&sn, &storage](const crow::request& req){
        auto x = crow::json::load(req.body);
        if (!x) return crow::response(400, "JSON invalido");

        int userId = x["userId"].i();
        int targetId = x["targetId"].i(); // Aqui usamos ID pq a notificação já sabe o ID de quem mandou
        std::string action = x["action"].s();

        try {
            Profile* user = sn.getProfile(userId);
            Profile* requester = sn.getProfile(targetId);

            if (action == "ACCEPT") {
                ContactHandlers::acceptFriendRequest(user, requester, sn, storage);
                return crow::response(200, "Pedido aceito! Agora sao amigos.");
            } 
            else if (action == "REFUSE") {
                ContactHandlers::refuseFriendRequest(user, requester, sn, storage);
                return crow::response(200, "Pedido recusado.");
            } 
            else {
                return crow::response(400, "Acao invalida. Use ACCEPT ou REFUSE.");
            }
        } catch (...) {
            return crow::response(404, "Erro ao processar pedido.");
        }
    });

    // ROTA CURTIR POST
    // JSON: { "userId": 1, "authorId": 2, "postIndex": 0 }
    CROW_ROUTE(app, "/api/posts/like").methods(crow::HTTPMethod::Post)
    ([&sn, &storage](const crow::request& req){
        auto x = crow::json::load(req.body);
        if (!x) return crow::response(400, "JSON invalido");

        int userId = x["userId"].i();
        int authorId = x["authorId"].i();
        int postIndex = x["postIndex"].i();

        try {
            Profile* liker = sn.getProfile(userId);
            Profile* author = sn.getProfile(authorId);
            auto* posts = author->getPosts(); 

            if (postIndex < 0 || postIndex >= (int)posts->size()) {
                return crow::response(404, "Post nao encontrado (indice invalido).");
            }

            auto it = posts->begin();
            std::advance(it, postIndex);
            Post* targetPost = *it;

            targetPost->addLike(liker);
            storage.save(&sn);

            return crow::response(200, "Post curtido!");
        } catch (const std::logic_error& e) {
            return crow::response(400, e.what()); // Ex: "Já curtiu antes"
        } catch (...) {
            return crow::response(404, "Erro ao processar like.");
        }
    });

    // ROTA COMENTAR
    // JSON: { "userId": 1, "authorId": 2, "postIndex": 0, "text": "Legal!" }
    CROW_ROUTE(app, "/api/posts/comment").methods(crow::HTTPMethod::Post)
    ([&sn, &storage](const crow::request& req){
        auto x = crow::json::load(req.body);
        if (!x) return crow::response(400, "JSON invalido");

        int userId = x["userId"].i();
        int authorId = x["authorId"].i();
        int postIndex = x["postIndex"].i();
        std::string text = x["text"].s();

        if (text.empty()) return crow::response(400, "Comentario vazio.");

        try {
            Profile* commenter = sn.getProfile(userId);
            Profile* author = sn.getProfile(authorId);

            auto* posts = author->getPosts();
            
            if (postIndex < 0 || postIndex >= (int)posts->size()) {
                return crow::response(404, "Post nao encontrado.");
            }

            auto it = posts->begin();
            std::advance(it, postIndex);
            Post* targetPost = *it;

            targetPost->addComment(text, commenter);
            storage.save(&sn);

            return crow::response(200, "Comentario adicionado!");
        } catch (...) {
            return crow::response(404, "Erro ao comentar.");
        }
    });

    // -------------------------------------------------------------------------
    // GRUPO: DADOS AUXILIARES (UI)
    // -------------------------------------------------------------------------

    // ROTA: MINHAS NOTIFICAÇÕES
    CROW_ROUTE(app, "/api/notifications/<int>")
    ([&sn](int userId){
        try {
            Profile* p = sn.getProfile(userId);
            User* u = dynamic_cast<User*>(p);
            
            if (!u) return crow::response(200, "[]"); 

            const auto& notifs = u->getNotifications();
            
            crow::json::wvalue jsonResp;
            int i = 0;
            
            for(const auto& n : notifs) {
                jsonResp[i]["text"] = n->getMessage();
                jsonResp[i]["date"] = n->getFormattedDate(); // Aproveitei que vi essa função no seu .h
                jsonResp[i]["read"] = n->isRead();
                i++;
            }
            return crow::response(200, jsonResp);
        } catch (...) {
            return crow::response(404, "Usuario nao encontrado");
        }
    });

    // ROTA: MEUS AMIGOS (Aba Amigos e Sidebar)
    CROW_ROUTE(app, "/api/friends/<int>")
    ([&sn](int userId){
        try {
            Profile* p = sn.getProfile(userId);
            auto* contacts = p->getContacts();

            crow::json::wvalue jsonResp;
            int i = 0;
            for(auto* c : *contacts) {
                jsonResp[i]["id"] = c->getId();
                jsonResp[i]["name"] = c->getName();
                jsonResp[i]["icon"] = c->getIconPath();
                jsonResp[i]["subtitle"] = c->getSubtitle(); // Bio curta ou Cargo
                i++;
            }
            return crow::response(200, jsonResp);
        } catch (...) {
            return crow::response(404, "Usuario nao encontrado");
        }
    });

    // ROTA: MINHAS PÁGINAS (Sidebar - Atalhos)
    CROW_ROUTE(app, "/api/pages/owned/<int>")
    ([&sn](int userId){
        // Como não temos uma lista direta "myPages" no User,
        // vamos varrer os perfis procurando páginas que esse user é dono.
        // (Não é o mais performático do mundo, mas para <1000 users é instantâneo)
        
        std::vector<Profile*> allProfiles = sn.searchProfiles(""); // Pega todos
        crow::json::wvalue jsonResp;
        int i = 0;

        for (auto* p : allProfiles) {
            Page* page = dynamic_cast<Page*>(p);
            if (page) {
                if (page->getOwner()->getId() == userId) {
                    jsonResp[i]["id"] = page->getId();
                    jsonResp[i]["name"] = page->getName();
                    jsonResp[i]["icon"] = page->getIconPath();
                    i++;
                }
            }
        }
        return crow::response(200, jsonResp);
    });

    // =================================================================================
    // 3. START DO SERVIDOR
    // =================================================================================
    std::cout << ">> Servidor rodando em http://127.0.0.1:8085\n";
    // Bind em 0.0.0.0 para evitar conflitos de IP no Windows
    app.bindaddr("0.0.0.0").port(8085).multithreaded().run();
    
    return 0;
}
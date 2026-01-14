#include "API/Router.h"
#include "Auth/User.h"
#include "Content/Post.h"
#include "Content/Comment.h"
#include "Content/Like.h" 
#include "Content/Notification.h"
#include "Core/Translation.h"
#include "Core/Utils.h"
#include "Core/Database.h"
#include "Core/Logger.h"
#include <vector>

namespace API {

    void Router::setupRoutes(crow::SimpleApp& app) {

        // ---------------------------------------------------------
        // ROTA 1: HEALTH CHECK
        // ---------------------------------------------------------
        CROW_ROUTE(app, "/health")([](){
            auto* tr = Core::Translation::getInstance();
            crow::json::wvalue x;
            x["status"] = "online";
            x["version"] = "1.0.0";
            x["message"] = tr->get("API_HEALTH_MSG");
            return x;
        });

        // ---------------------------------------------------------
        // ROTA 2: CADASTRO
        // ---------------------------------------------------------
        CROW_ROUTE(app, "/api/signup").methods(crow::HTTPMethod::Post)
        ([](const crow::request& req){
            auto* tr = Core::Translation::getInstance();
            auto x = crow::json::load(req.body);
            
            if (!x) return crow::response(400, tr->get("ERR_JSON"));

            Auth::User newUser;
            if (!x.has("username") || !x.has("email") || !x.has("password")) {
                 return crow::response(400, tr->get("ERR_MISSING"));
            }

            newUser.setUsername(x["username"].s());
            newUser.setEmail(x["email"].s());
            newUser.setPassword(x["password"].s());
            
            if (x.has("birth_date")) {
                try {
                    std::string validDate = Core::Utils::validateISO(x["birth_date"].s());
                    newUser.setBirthDate(validDate); 
                } catch (const std::exception& e) {
                    std::string errorMsg = tr->get("ERR_PREFIX_DATE") + std::string(e.what());
                    return crow::response(400, errorMsg);
                }
            }

            if (newUser.save()) {
                return crow::response(201, tr->get("MSG_CREATED"));
            } else {
                return crow::response(409, tr->get("ERR_CONFLICT"));
            }
        });

        // ---------------------------------------------------------
        // ROTA 3: LOGIN
        // ---------------------------------------------------------
        CROW_ROUTE(app, "/api/login").methods(crow::HTTPMethod::Post)
        ([](const crow::request& req){
            auto* tr = Core::Translation::getInstance();
            auto x = crow::json::load(req.body);

            if (!x) return crow::response(400, tr->get("ERR_JSON"));

            std::string email = x["email"].s();
            std::string pass  = x["password"].s();

            Auth::User user;
            if (Auth::User::findByEmail(email, user)) {
                if (user.checkPassword(pass)) {
                    crow::json::wvalue response;
                    response["id"] = user.getId();
                    response["username"] = user.getUsername();
                    response["msg"] = tr->get("MSG_LOGIN_OK");
                    return crow::response(200, response);
                } else {
                    return crow::response(401, tr->get("ERR_WRONG_PASS"));
                }
            }
            
            return crow::response(404, tr->get("ERR_USER_NOT_FOUND"));
        });

        // ---------------------------------------------------------
        // ROTA 4: CRIAR POST
        // ---------------------------------------------------------
        CROW_ROUTE(app, "/api/posts").methods(crow::HTTPMethod::Post)
        ([](const crow::request& req){
             auto* tr = Core::Translation::getInstance();
             auto x = crow::json::load(req.body);

             if (!x) return crow::response(400, tr->get("ERR_JSON"));

             if (!x.has("author_id") || !x.has("content")) {
                 return crow::response(400, tr->get("ERR_MISSING"));
             }

             Content::Post newPost;
             newPost.setAuthorId(x["author_id"].i());
             newPost.setContent(x["content"].s());

             if (newPost.save()) {
                 return crow::response(201, tr->get("MSG_POST_CREATED"));
             } else {
                 return crow::response(500, tr->get("SQL_ERROR"));
             }
        });

        // ---------------------------------------------------------
        // ROTA 5: LER FEED (Com Contagem de Likes)
        // ---------------------------------------------------------
        CROW_ROUTE(app, "/api/feed/<int>")
        ([](int userId){
            std::vector<Content::Post> posts = Content::Post::getPostsByUserId(userId);
            
            std::vector<crow::json::wvalue> jsonList;
            
            for (const auto& p : posts) {
                crow::json::wvalue postJson;
                postJson["id"] = p.getId();
                postJson["content"] = p.getContent();
                postJson["date"] = p.getCreationDate();
                
                // --- INTEGRANDO LIKES ---
                postJson["likes_count"] = Content::Like::getCount(p.getId());
                postJson["i_liked"] = Content::Like::hasUserLiked(p.getId(), userId);
                // ------------------------

                jsonList.push_back(postJson);
            }

            return crow::json::wvalue(jsonList);
        });

        // ---------------------------------------------------------
        // ROTA 6: EDITAR PERFIL
        // ---------------------------------------------------------
        CROW_ROUTE(app, "/api/users/<int>").methods(crow::HTTPMethod::Put)
        ([](const crow::request& req, int userId){
            auto* tr = Core::Translation::getInstance();
            auto x = crow::json::load(req.body);
            
            if (!x) return crow::response(400, tr->get("ERR_JSON"));

            Auth::User user;
            if (!Auth::User::findById(userId, user)) {
                return crow::response(404, tr->get("ERR_USER_NOT_FOUND"));
            }

            if (x.has("bio")) user.setBio(x["bio"].s());
            if (x.has("language")) user.setLanguage(x["language"].s());
            if (x.has("birth_date")) {
                try {
                    std::string validDate = Core::Utils::validateISO(x["birth_date"].s());
                    user.setBirthDate(validDate); 
                } catch (const std::exception& e) {
                    std::string errorMsg = tr->get("ERR_PREFIX_DATE") + std::string(e.what());
                    return crow::response(400, errorMsg);
                }
            }
            
            if (user.update()) {
                return crow::response(200, tr->get("MSG_PROFILE_UPDATED"));
            } else {
                return crow::response(500, tr->get("SQL_ERROR"));
            }
        });

        // ---------------------------------------------------------
        // ROTA 7: BUSCAR USUÁRIOS
        // ---------------------------------------------------------
        CROW_ROUTE(app, "/api/search")
        ([](const crow::request& req){
            char* queryParam = req.url_params.get("q");
            
            if (!queryParam) return crow::json::wvalue(std::vector<crow::json::wvalue>{});

            std::vector<Auth::User> results = Auth::User::search(std::string(queryParam));
            
            std::vector<crow::json::wvalue> jsonList;
            for (const auto& u : results) {
                crow::json::wvalue item;
                item["id"] = u.getId();
                item["username"] = u.getUsername();
                item["bio"] = u.getBio();
                jsonList.push_back(item);
            }

            return crow::json::wvalue(jsonList);
        });

        // ---------------------------------------------------------
        // ROTA 8.1: ENVIAR SOLICITAÇÃO (Friend Request)
        // ---------------------------------------------------------
        CROW_ROUTE(app, "/api/friends/request").methods(crow::HTTPMethod::Post)
        ([](const crow::request& req){
            auto* tr = Core::Translation::getInstance();
            auto x = crow::json::load(req.body);
            
            if (!x) return crow::response(400, tr->get("ERR_JSON"));
            if (!x.has("from_id") || !x.has("to_id")) {
                return crow::response(400, tr->get("ERR_MISSING"));
            }

            Auth::User me, other;
            me.setId(x["from_id"].i());
            other.setId(x["to_id"].i());

            if (me.sendFriendRequest(&other)) {
                return crow::response(200, tr->get("MSG_REQ_SENT")); 
            } else {
                return crow::response(409, tr->get("ERR_REQ_EXIST"));
            }
        });

        // ---------------------------------------------------------
        // ROTA 8.2: LISTAR PENDENTES (Minhas Notificações)
        // ---------------------------------------------------------
        CROW_ROUTE(app, "/api/friends/pending/<int>")
        ([](int myId){
            Auth::User me;
            me.setId(myId);
            std::vector<Auth::User> list = me.getPendingRequests();

            std::vector<crow::json::wvalue> jsonList;
            for (const auto& u : list) {
                crow::json::wvalue item;
                item["id"] = u.getId();
                item["username"] = u.getUsername();
                jsonList.push_back(item);
            }
            return crow::json::wvalue(jsonList);
        });

        // ---------------------------------------------------------
        // ROTA 8.3: RESPONDER (Aceitar/Recusar)
        // ---------------------------------------------------------
        CROW_ROUTE(app, "/api/friends/respond").methods(crow::HTTPMethod::Post)
        ([](const crow::request& req){
            auto* tr = Core::Translation::getInstance();
            auto x = crow::json::load(req.body);
            
            if (!x) return crow::response(400, tr->get("ERR_JSON"));
            if (!x.has("user_id") || !x.has("requester_id") || !x.has("action")) {
                return crow::response(400, tr->get("ERR_MISSING"));
            }

            std::string action = x["action"].s();
            Auth::User me, requester;
            me.setId(x["user_id"].i());
            requester.setId(x["requester_id"].i());

            bool success = false;
            if (action == "accept") {
                success = me.acceptFriendRequest(&requester);
            } else if (action == "reject") {
                success = me.removeOrRejectFriend(&requester);
            }

            if (success) {
                return crow::response(200, tr->get("MSG_REQ_PROCESSED"));
            } else {
                return crow::response(500, tr->get("ERR_REQ_PROCESS"));
            }
        });

        // ---------------------------------------------------------
        // ROTA 9: COMENTAR
        // ---------------------------------------------------------
        CROW_ROUTE(app, "/api/comments").methods(crow::HTTPMethod::Post)
        ([](const crow::request& req){
            auto* tr = Core::Translation::getInstance();
            auto x = crow::json::load(req.body);
            
            if (!x) return crow::response(400, tr->get("ERR_JSON"));
            if (!x.has("post_id") || !x.has("author_id") || !x.has("content")) {
                return crow::response(400, tr->get("ERR_MISSING"));
            }

            if (x["content"].s().size() == 0) {
                return crow::response(400, tr->get("ERR_COMMENT_EMPTY"));
            }

            Content::Comment c;
            c.setPostId(x["post_id"].i());
            c.setAuthorId(x["author_id"].i());
            c.setContent(x["content"].s());
            
            if (x.has("parent_id")) {
                c.setParentId(x["parent_id"].i());
            }

            if (c.save()) {
                return crow::response(201, tr->get("MSG_COMMENT_ADDED")); 
            } else {
                return crow::response(500, tr->get("SQL_ERROR"));
            }
        });

        // ---------------------------------------------------------
        // ROTA 10: LER COMENTÁRIOS DE UM POST
        // ---------------------------------------------------------
        CROW_ROUTE(app, "/api/posts/<int>/comments")
        ([](int postId){
            std::vector<Content::Comment> comments = Content::Comment::getCommentsByPostId(postId);
            
            std::vector<crow::json::wvalue> jsonList;
            for (const auto& c : comments) {
                crow::json::wvalue item;
                item["id"] = c.getId();
                item["author_id"] = c.getAuthorId();
                item["content"] = c.getContent();
                item["date"] = c.getCreationDate();
                
                if (c.getParentId() != -1) {
                    item["parent_id"] = c.getParentId();
                }
                
                jsonList.push_back(item);
            }

            return crow::json::wvalue(jsonList);
        });

        // ---------------------------------------------------------
        // ROTA 11: DAR/TIRAR LIKE (Toggle)
        // ---------------------------------------------------------
        CROW_ROUTE(app, "/api/likes").methods(crow::HTTPMethod::Post)
        ([](const crow::request& req){
            auto* tr = Core::Translation::getInstance();
            auto x = crow::json::load(req.body);
            
            if (!x) return crow::response(400, tr->get("ERR_JSON"));
            if (!x.has("post_id") || !x.has("user_id")) {
                return crow::response(400, tr->get("ERR_MISSING"));
            }

            Content::Like like;
            like.setPostId(x["post_id"].i());
            like.setUserId(x["user_id"].i());

            bool wasAdded = like.toggle();

            if (wasAdded) {
                return crow::response(201, tr->get("MSG_LIKE_ADDED")); 
            } else {
                return crow::response(200, tr->get("MSG_LIKE_REMOVED"));
            }
        });

        // ---------------------------------------------------------
        // ROTA 12: CENTRAL DE NOTIFICAÇÕES
        // GET /api/notifications/<my_id>
        // ---------------------------------------------------------
        CROW_ROUTE(app, "/api/notifications/<int>")
        ([](int userId){
            // Retorna lista rica com is_read, sender, data, texto traduzido
            std::vector<crow::json::wvalue> list = Content::Notification::getByUser(userId);
            return crow::json::wvalue(list);
        });

        // ---------------------------------------------------------
        // ROTA 13: MARCAR NOTIFICAÇÕES COMO LIDAS
        // POST /api/notifications/read
        // JSON: { "user_id": 1 }
        // ---------------------------------------------------------
        CROW_ROUTE(app, "/api/notifications/read").methods(crow::HTTPMethod::Post)
        ([](const crow::request& req){
            auto x = crow::json::load(req.body);
            if (!x || !x.has("user_id")) return crow::response(400);
            
            Content::Notification::markAllAsRead(x["user_id"].i());
            return crow::response(200, "OK");
        });

        // ROTA 14: LOG DE ATIVIDADES
        CROW_ROUTE(app, "/api/logs/<int>")
        ([](int userId){
            // Agora chamamos a classe correta
            auto logs = Core::Logger::getUserLogs(userId);
            return crow::json::wvalue(logs);
        });
    }
}
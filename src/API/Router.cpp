#include "API/Router.h"
#include "Auth/User.h"
#include "Content/Post.h"
#include "Content/Comment.h"
#include "Content/Like.h" 
#include "Content/Notification.h"
#include "Social/Community.h"
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
        // ROTA 3.1: EXCLUIR CONTA
        // ---------------------------------------------------------
        CROW_ROUTE(app, "/api/users/<int>").methods(crow::HTTPMethod::Delete)
        ([](int userId){
            auto* tr = Core::Translation::getInstance();
            Auth::User user;
            if (!Auth::User::findById(userId, user)) {
                return crow::response(404, tr->get("ERR_USER_NOT_FOUND"));
            }

            if (user.deleteAccount(userId)) {
                return crow::response(200, tr->get("MSG_USER_DELETED"));
            } else {
                return crow::response(400, tr->get("ERR_DELETE_MASTER"));
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
        // ROTA 4: CRIAR POST (Atualizada para suportar comunidades)
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

            if (x.has("community_id")) {
                newPost.setCommunityId(x["community_id"].i());
            }

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
            auto* db = Core::Database::getInstance();
            std::vector<crow::json::wvalue> jsonList;

            // SQL com JOIN para pegar o nome da comunidade, se existir
            std::string sql = "SELECT p.id, p.content, p.creation_date, p.community_id, c.name "
                            "FROM posts p "
                            "LEFT JOIN communities c ON p.community_id = c.id "
                            "WHERE p.author_id = " + std::to_string(userId) + " ORDER BY p.id DESC;";

            db->query(sql, [&](int argc, char** argv, char** colNames) {
                crow::json::wvalue postJson;
                postJson["id"] = std::stoi(argv[0]);
                postJson["content"] = argv[1];
                postJson["date"] = argv[2];
                
                // Identificação da Comunidade
                if (argv[3] != nullptr) {
                    postJson["community_id"] = std::stoi(argv[3]);
                    postJson["community_name"] = argv[4]; 
                    postJson["is_community_post"] = true;
                } else {
                    postJson["is_community_post"] = false;
                }
                postJson["likes_count"] = Content::Like::getCount(std::stoi(argv[0]));
                jsonList.push_back(std::move(postJson));
                return 0;
            });

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

        // ROTA 15: CRIAR COMUNIDADE
        CROW_ROUTE(app, "/api/communities").methods(crow::HTTPMethod::Post)
        ([](const crow::request& req){
            auto* tr = Core::Translation::getInstance();
            auto x = crow::json::load(req.body);
            if (!x || !x.has("name") || !x.has("owner_id")) return crow::response(400, tr->get("ERR_MISSING"));

            Social::Community comm;
            comm.setName(x["name"].s());
            comm.setDescription(x.has("description") ? std::string(x["description"].s()) : std::string(""));
            comm.setOwnerId(x["owner_id"].i());

            if (comm.save()) {
                return crow::response(201, tr->get("MSG_COMM_CREATED"));
            }
            return crow::response(500, tr->get("SQL_ERROR"));
        });

        // ---------------------------------------------------------
        // ROTA 15.2: SOLICITAR ENTRADA (O que deu 404)
        // ---------------------------------------------------------
        CROW_ROUTE(app, "/api/communities/request").methods(crow::HTTPMethod::Post)
        ([](const crow::request& req){
            auto* tr = Core::Translation::getInstance();
            auto x = crow::json::load(req.body);
            if (!x || !x.has("community_id") || !x.has("user_id")) return crow::response(400, tr->get("ERR_MISSING"));

            Social::Community comm;
            comm.setId(x["community_id"].i());

            if (comm.requestJoin(x["user_id"].i())) {
                return crow::response(200, tr->get("MSG_REQ_SENT"));
            }
            return crow::response(400, tr->get("ERR_REQ_EXIST"));
        });

        CROW_ROUTE(app, "/api/communities/approve").methods(crow::HTTPMethod::Post)
        ([](const crow::request& req){
            auto* tr = Core::Translation::getInstance();
            auto x = crow::json::load(req.body);
            
            if (!x || !x.has("community_id") || !x.has("user_id") || !x.has("admin_id")) {
                return crow::response(400, tr->get("ERR_MISSING"));
            }

            int commId = x["community_id"].i();
            int targetUserId = x["user_id"].i();
            int adminId = x["admin_id"].i(); // Quem está tentando aprovar

            if (!Social::Community::checkPermission(commId, adminId, Social::CommunityRole::ADMIN)) {
                return crow::response(403, tr->get("ERR_PERMISSION_DENIED"));
            }

            if (Social::Community::addMember(commId, targetUserId, Social::CommunityRole::MEMBER)) {
                auto* db = Core::Database::getInstance();
                
                db->execute("DELETE FROM community_requests WHERE community_id = " + std::to_string(commId) + 
                            " AND user_id = " + std::to_string(targetUserId));
                
                return crow::response(200, tr->get("APROVE_COMM_MEMBER"));
            } 

            return crow::response(400, tr->get("ERR_ALREADY_MEMBER"));
        });

        // ROTA 15.1: EXCLUIR COMUNIDADE (DELETE)
        // Apenas o Master Admin pode destruir a página
        CROW_ROUTE(app, "/api/communities").methods(crow::HTTPMethod::Delete)
        ([](const crow::request& req){
            auto* tr = Core::Translation::getInstance();
            auto x = crow::json::load(req.body);
            
            if (!x || !x.has("community_id") || !x.has("admin_id")) {
                return crow::response(400, tr->get("ERR_MISSING"));
            }

            Social::Community comm;
            comm.setId(x["community_id"].i());

            if (comm.destroy(x["admin_id"].i())) {
                return crow::response(200, tr->get("MSG_COMM_DELETED"));
            }
            return crow::response(403, tr->get("ERR_NOT_MASTER"));
        });

        // ROTA 16: GERENCIAR ROLES (Promover/Rebaixar)
        CROW_ROUTE(app, "/api/communities/role").methods(crow::HTTPMethod::Post)
        ([](const crow::request& req){
            auto* tr = Core::Translation::getInstance();
            auto x = crow::json::load(req.body);
            
            int commId = x["community_id"].i();
            int masterId = x["master_id"].i(); 
            int targetId = x["target_id"].i(); 
            int newRole = x["new_role"].i();   

            // Validação de Segurança: Apenas o Master (Role 1) pode gerenciar outros ADMs
            if (!Social::Community::checkPermission(commId, masterId, Social::CommunityRole::MASTER_ADMIN)) {
                return crow::response(403, tr->get("ERR_PERMISSION_DENIED"));
            }

            auto* db = Core::Database::getInstance();
            std::string sql = "UPDATE community_members SET role = " + std::to_string(newRole) +
                            " WHERE community_id = " + std::to_string(commId) + 
                            " AND user_id = " + std::to_string(targetId) + ";";

            if (db->execute(sql)) {
                return crow::response(200, tr->get("MSG_ROLE_UPDATED"));
            }
            return crow::response(500, tr->get("SQL_ERROR"));
        });

        CROW_ROUTE(app, "/api/communities/remove_member").methods(crow::HTTPMethod::Post)
        ([](const crow::request& req){
            auto* tr = Core::Translation::getInstance();
            auto x = crow::json::load(req.body);
            
            if (!x.has("community_id") || !x.has("admin_id") || !x.has("target_id")) {
                return crow::response(400, tr->get("ERR_MISSING"));
            }

            Social::Community comm;
            comm.setId(x["community_id"].i());

            if (comm.removeMember(x["target_id"].i(), x["admin_id"].i())) {
                return crow::response(200, tr->get("MSG_MEMBER_REMOVED"));
            }
            
            return crow::response(403, tr->get("ERR_PERMISSION_DENIED"));
        });

        // ROTA 17: TIMELINE DA COMUNIDADE
        CROW_ROUTE(app, "/api/communities/<int>/posts")
        ([](int commId){
            auto* db = Core::Database::getInstance();
            std::vector<crow::json::wvalue> postsList;

            std::string sql = "SELECT p.id, p.author_id, p.content, p.creation_date, u.username "
                            "FROM posts p JOIN users u ON p.author_id = u.id "
                            "WHERE p.community_id = " + std::to_string(commId) + 
                            " ORDER BY p.id DESC;";

            auto callback = [&](int argc, char** argv, char**) -> int {
                crow::json::wvalue p;
                p["id"] = std::stoi(argv[0]);
                p["author_id"] = std::stoi(argv[1]);
                p["content"] = argv[2] ? argv[2] : "";
                p["date"] = argv[3] ? argv[3] : "";
                p["author_name"] = argv[4] ? argv[4] : "Unknown";
                p["likes_count"] = Content::Like::getCount(std::stoi(argv[0]));
                
                postsList.push_back(std::move(p));
                return 0;
            };

            db->query(sql, callback);
            return crow::json::wvalue(postsList);
        });

        // ROTA 18: HOME PERSONALIZADA (Meus posts + Amigos + Comunidades)
        CROW_ROUTE(app, "/api/home/<int>")
        ([](int userId){
            auto* db = Core::Database::getInstance();
            std::vector<crow::json::wvalue> posts;

            std::string sql = 
                "SELECT p.id, p.content, p.creation_date, u.username, "
                "COALESCE(c.name, 'Personal') as origin, "
                "(SELECT COUNT(*) FROM likes l WHERE l.post_id = p.id) as likes_count, "
                "(SELECT COUNT(*) FROM comments cm WHERE cm.post_id = p.id) as comments_count "
                "FROM posts p "
                "JOIN users u ON p.author_id = u.id "
                "LEFT JOIN communities c ON p.community_id = c.id "
                "WHERE p.author_id = " + std::to_string(userId) + " "
                "OR p.author_id IN (SELECT user_id_1 FROM friendships WHERE user_id_2 = " + std::to_string(userId) + " AND status = 1 "
                "                   UNION SELECT user_id_2 FROM friendships WHERE user_id_1 = " + std::to_string(userId) + " AND status = 1) "
                "OR p.community_id IN (SELECT community_id FROM community_members WHERE user_id = " + std::to_string(userId) + ") "
                "ORDER BY p.id DESC LIMIT 50;";

            db->query(sql, [&](int argc, char** argv, char**) {
                crow::json::wvalue p;
                p["id"] = std::stoi(argv[0]);
                p["content"] = argv[1];
                p["date"] = argv[2];
                p["author"] = argv[3];
                p["origin"] = argv[4];
                p["likes"] = std::stoi(argv[5]);
                p["comments"] = std::stoi(argv[6]);
                posts.push_back(std::move(p));
                return 0;
            });

            return crow::json::wvalue(posts);
        });

        // ROTA 19: SAIR DE UMA COMUNIDADE
        CROW_ROUTE(app, "/api/communities/leave").methods(crow::HTTPMethod::Post)
        ([](const crow::request& req){
            auto* tr = Core::Translation::getInstance();
            auto x = crow::json::load(req.body);
            int commId = x["community_id"].i();
            int userId = x["user_id"].i();

            Social::Community comm;
            comm.setId(commId);

            // TRAVA: Master não pode sair sem transferir
            if (!comm.canLeave(userId)) {
                return crow::response(403, tr->get("ERR_MASTER_LEAVE"));
            }

            auto* db = Core::Database::getInstance();
            std::string sql = "DELETE FROM community_members WHERE community_id = " + 
                            std::to_string(commId) + " AND user_id = " + std::to_string(userId) + ";";
            
            if (db->execute(sql)) return crow::response(200, tr->get("MSG_LEAVE_COMM"));
            return crow::response(500, tr->get("ERR_LEAVE_COMM"));
        });

        // ROTA 19.1: TRANSFERIR POSSE
        // Transfere o cargo de Master Admin para outro membro
        CROW_ROUTE(app, "/api/communities/transfer").methods(crow::HTTPMethod::Post)
        ([](const crow::request& req){
            auto* tr = Core::Translation::getInstance();
            auto x = crow::json::load(req.body);
            
            if (!x || !x.has("community_id") || !x.has("current_master") || !x.has("new_master")) {
                return crow::response(400, tr->get("ERR_MISSING"));
            }

            Social::Community comm;
            comm.setId(x["community_id"].i());

            if (comm.transferOwnership(x["current_master"].i(), x["new_master"].i())) {
                return crow::response(200, tr->get("MSG_TRANSFER_OK"));
            }
            return crow::response(403, tr->get("ERR_NOT_MASTER"));
        });

        // ROTA 20: LISTAR MEMBROS (Estilo Lista Lateral do Discord)
        CROW_ROUTE(app, "/api/communities/<int>/members")
        ([](int commId){
            std::vector<Auth::User> members = Social::Community::getRichMembers(commId);
            std::vector<crow::json::wvalue> jsonList;

            for (const auto& u : members) {
                crow::json::wvalue item;
                item["id"] = u.getId();
                item["username"] = u.getUsername();
                item["bio"] = u.getBio();
                // Aqui você pode adicionar o Role também usando getUserRole
                item["role"] = Social::Community::getUserRole(commId, u.getId());
                jsonList.push_back(std::move(item));
            }
            return crow::json::wvalue(jsonList);
        });

        // ROTA 21: BUSCAR MEMBROS DENTRO DA COMUNIDADE
        CROW_ROUTE(app, "/api/communities/<int>/members/search")
        ([](const crow::request& req, int commId){
            char* q = req.url_params.get("q");
            if (!q) return crow::json::wvalue(std::vector<crow::json::wvalue>{});

            std::vector<Auth::User> results = Social::Community::searchMembers(commId, std::string(q));
            std::vector<crow::json::wvalue> jsonList;

            for (const auto& u : results) {
                crow::json::wvalue item;
                item["id"] = u.getId();
                item["username"] = u.getUsername();
                jsonList.push_back(std::move(item));
            }
            return crow::json::wvalue(jsonList);
        });

        // ROTA 22: LISTAR PEDIDOS DE ENTRADA PENDENTES
        CROW_ROUTE(app, "/api/communities/<int>/requests")
        ([](int commId){
            // Retorna a lista de usuários na "sala de espera"
            std::vector<Auth::User> pending = Social::Community::getPendingRequests(commId);
            std::vector<crow::json::wvalue> jsonList;

            for (const auto& u : pending) {
                crow::json::wvalue item;
                item["id"] = u.getId();
                item["username"] = u.getUsername();
                item["bio"] = u.getBio();
                jsonList.push_back(std::move(item));
            }
            return crow::json::wvalue(jsonList);
        });

        // ROTA 23: PROCESSAR PEDIDO (ACEITAR OU RECUSAR)
        CROW_ROUTE(app, "/api/communities/respond_request").methods(crow::HTTPMethod::Post)
        ([](const crow::request& req){
            auto* tr = Core::Translation::getInstance();
            auto x = crow::json::load(req.body);
            
            if (!x || !x.has("community_id") || !x.has("user_id") || !x.has("action")) {
                return crow::response(400, tr->get("ERR_MISSING"));
            }

            int commId = x["community_id"].i();
            int targetUserId = x["user_id"].i();
            std::string action = x["action"].s(); // "accept" ou "reject"

            auto* db = Core::Database::getInstance();

            if (action == "accept") {
                // Se aceitar, usa a chave MSG_REQ_ACCEPTED do Translation
                if (Social::Community::addMember(commId, targetUserId, Social::CommunityRole::MEMBER)) {
                    db->execute("DELETE FROM community_requests WHERE community_id = " + std::to_string(commId) + 
                                " AND user_id = " + std::to_string(targetUserId));
                    return crow::response(200, tr->get("MSG_REQ_ACCEPTED"));
                }
            } else if (action == "reject") {
                // Se recusar, usa a chave MSG_REQ_REJECTED do Translation
                db->execute("DELETE FROM community_requests WHERE community_id = " + std::to_string(commId) + 
                            " AND user_id = " + std::to_string(targetUserId));
                return crow::response(200, tr->get("MSG_REQ_REJECTED"));
            }

            return crow::response(400, tr->get("ERR_PROCESS_REQ"));
        });

        // ROTA DE DEBUG: Ver o que tem no banco de dados (Comunidades e Posts)
        // CROW_ROUTE(app, "/api/debug/db")
        // ([](){
        //     auto* db = Core::Database::getInstance();
        //     std::cout << "\n--- [DEBUG BANCO DE DADOS] ---" << std::endl;
            
        //     std::cout << "\n[MEMBROS DA COMUNIDADE]:" << std::endl;
        //     db->query("SELECT community_id, user_id, role FROM community_members;", [](int argc, char** argv, char**) {
        //         std::cout << "CommID: " << argv[0] << " | UserID: " << argv[1] << " | Role: " << argv[2] << std::endl;
        //         return 0;
        //     });

        //     std::cout << "\n[POSTS GRAVADOS]:" << std::endl;
        //     db->query("SELECT id, author_id, community_id, content FROM posts;", [](int argc, char** argv, char**) {
        //         std::cout << "ID: " << argv[0] << " | Auth: " << argv[1] << " | Comm: " << (argv[2] ? argv[2] : "NULL") << " | Cont: " << argv[3] << std::endl;
        //         return 0;
        //     });

        //     return crow::response(200, "Dados impressos no console do servidor!");
        // });
    }
}
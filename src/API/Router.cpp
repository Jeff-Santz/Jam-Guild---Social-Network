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
#include "Core/TokenService.h"
#include <vector>
#include <algorithm>
#include <cctype>

namespace API {

    // --- O GUARDIÃO DA SEGURANÇA ---
    int Router::authenticate(const crow::request& req) {
        std::string authHeader = req.get_header_value("Authorization");
        if (authHeader.empty() || authHeader.substr(0, 7) != "Bearer ") return -1;
        return Core::TokenService::verifyToken(authHeader.substr(7));
    }

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

            if (x.has("is_private")) {
                newUser.setPrivate(x["is_private"].b());
            }

            if (newUser.save()) {
                return crow::response(201, tr->get("MSG_CREATED"));
            } else {
                return crow::response(409, tr->get("ERR_CONFLICT"));
            }
        });

        // ---------------------------------------------------------
        // ROTA AUXILIAR: SIMULAR VALIDAÇÃO DE EMAIL
        // ---------------------------------------------------------
        CROW_ROUTE(app, "/api/verify_me").methods(crow::HTTPMethod::Post)
        ([](const crow::request& req){
            auto* tr = Core::Translation::getInstance(); // Necessário instanciar!
            
            int userId = Router::authenticate(req);
            if (userId == -1) return crow::response(401, tr->get("ERR_AUTH_FAILED"));

            Auth::User user;
            user.setId(userId);
            
            if (user.markEmailAsVerified()) {
                return crow::response(200, tr->get("MSG_EMAIL_VERIFIED"));
            }
            
            return crow::response(500, tr->get("SQL_ERROR"));
        });

        // ---------------------------------------------------------
        // ROTA 3.1: EXCLUIR CONTA
        // ---------------------------------------------------------
        CROW_ROUTE(app, "/api/user").methods(crow::HTTPMethod::Delete)
        ([](const crow::request& req){
            auto* tr = Core::Translation::getInstance();
            int userId = Router::authenticate(req); // <--- BLINDAGEM
            if (userId == -1) return crow::response(401, tr->get("ERR_AUTH_FAILED"));

            Auth::User user;
            if (user.deleteAccount(userId)) {
                return crow::response(200, tr->get("MSG_USER_DELETED"));
            }
            return crow::response(400, tr->get("ERR_DELETE_MASTER"));
        });

        // ---------------------------------------------------------
        // ROTA 3: LOGIN
        // ---------------------------------------------------------
        CROW_ROUTE(app, "/api/login").methods(crow::HTTPMethod::Post)
        ([](const crow::request& req) {
            auto* tr = Core::Translation::getInstance();
            auto x = crow::json::load(req.body);

            if (!x || !x.has("identifier") || !x.has("password")) {
                return crow::response(400, tr->get("ERR_JSON"));
            }

            std::string identifier = x["identifier"].s();
            std::string password = x["password"].s();

            Auth::User user;
            if (user.login(identifier, password)) {
                std::string token = Core::TokenService::createToken(user.getId());

                crow::json::wvalue res;
                res["status"] = "success";
                res["token"] = token;
                res["username"] = user.getUsername();

                return crow::response(200, res);
            } else {
                return crow::response(401, tr->get("ERR_AUTH_FAILED"));
            }
        });

        // ---------------------------------------------------------
        // ROTA 4: CRIAR POST (Atualizada para suportar comunidades)
        // ---------------------------------------------------------
        CROW_ROUTE(app, "/api/posts").methods(crow::HTTPMethod::Post)
        ([](const crow::request& req){
            auto* tr = Core::Translation::getInstance();
            int userId = Router::authenticate(req);
            if (userId == -1) return crow::response(401, tr->get("ERR_AUTH_FAILED"));

            auto x = crow::json::load(req.body);
            if (!x || !x.has("content")) return crow::response(400, tr->get("ERR_MISSING"));

            Content::Post newPost;
            newPost.setAuthorId(userId); // <--- GARANTIA DE AUTORIA
            newPost.setContent(x["content"].s());

            if (x.has("community_id")) newPost.setCommunityId(x["community_id"].i());
            if (x.has("tags")) newPost.setTags(x["tags"].s()); // Adicione o tolower se quiser

            if (newPost.save()) return crow::response(201, tr->get("MSG_POST_CREATED"));
            return crow::response(500, tr->get("SQL_ERROR"));
        });

        // ---------------------------------------------------------
        // ROTA 5: LER FEED (Com Contagem de Likes)
        // ---------------------------------------------------------
        CROW_ROUTE(app, "/api/feed")
        ([](const crow::request& req) {
            int userId = Router::authenticate(req);
            if (userId == -1) return crow::response(401);
            auto* tr = Core::Translation::getInstance();
            auto* db = Core::Database::getInstance();
            
            char* viewerParam = req.url_params.get("viewer");
            int viewerId = viewerParam ? std::stoi(viewerParam) : userId;

            std::string sql = 
                "SELECT p.id, p.content, u.username, COALESCE(c.name, 'Pessoal') as origin, "
                "(SELECT COUNT(*) FROM likes l WHERE l.post_id = p.id) as likes_count, "
                "CASE "
                "   WHEN u.id IN (SELECT user_id_1 FROM friendships WHERE user_id_2 = " + std::to_string(viewerId) + " AND status = 1 "
                "                 UNION SELECT user_id_2 FROM friendships WHERE user_id_1 = " + std::to_string(viewerId) + " AND status = 1) THEN 50 " 
                "   WHEN p.community_id IN (SELECT community_id FROM community_members WHERE user_id = " + std::to_string(viewerId) + ") THEN 50 " 
                "   ELSE 0 "
                "END + "
                "((SELECT COUNT(*) FROM likes l WHERE l.post_id = p.id) * 2) + " 
                "COALESCE((SELECT SUM(ui.weight) FROM user_interests ui WHERE ui.user_id = " + std::to_string(viewerId) + " AND p.tags LIKE '%' || ui.tag || '%'), 0) * 10 " // Bônus de Tags
                "as algorithm_score "
                "FROM posts p "
                "JOIN users u ON p.author_id = u.id "
                "LEFT JOIN communities c ON p.community_id = c.id "
                "WHERE (u.is_private = 0 OR u.id = " + std::to_string(viewerId) + " OR u.id IN (SELECT user_id_1 FROM friendships WHERE user_id_2 = " + std::to_string(viewerId) + " AND status = 1)) " // Privacidade 
                "ORDER BY algorithm_score DESC LIMIT 50;"; 

            std::vector<crow::json::wvalue> feedList;
            db->query(sql, [&](int argc, char** argv, char**) {
                crow::json::wvalue p;
                p["id"] = std::stoi(argv[0]);
                p["content"] = argv[1];
                p["author"] = argv[2];
                p["origin"] = argv[3];
                p["likes"] = std::stoi(argv[4]);
                feedList.push_back(std::move(p));
                return 0;
            });

            return crow::response(crow::json::wvalue(feedList));
        });

        // ---------------------------------------------------------
        // ROTA 6: EDITAR PERFIL
        // ---------------------------------------------------------
        CROW_ROUTE(app, "/api/profile").methods(crow::HTTPMethod::Put)
        ([](const crow::request& req){
            auto* tr = Core::Translation::getInstance();
            int userId = Router::authenticate(req); // <--- BLINDAGEM
            if (userId == -1) return crow::response(401, tr->get("ERR_AUTH_FAILED"));

            auto x = crow::json::load(req.body);
            if (!x) return crow::response(400, tr->get("ERR_JSON"));

            Auth::User user;
            user.setId(userId); // Carrega o ID do token

            if (x.has("bio")) user.setBio(x["bio"].s());
            if (x.has("language")) user.setLanguage(x["language"].s());
            if (x.has("birth_date")) {
                try {
                    user.setBirthDate(Core::Utils::validateISO(x["birth_date"].s()));
                } catch (...) { return crow::response(400, tr->get("ERR_DATE_FORMAT")); }
            }
            
            if (user.update()) return crow::response(200, tr->get("MSG_PROFILE_UPDATED"));
            return crow::response(500, tr->get("SQL_ERROR"));
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
            int myId = Router::authenticate(req);
            auto* tr = Core::Translation::getInstance();
            if (myId == -1) return crow::response(401);

            auto x = crow::json::load(req.body);
            if (!x || !x.has("to_id")) return crow::response(400);

            Auth::User me, other;
            me.setId(myId);
            other.setId(x["to_id"].i());

            if (me.sendFriendRequest(&other)) return crow::response(200, tr->get("MSG_REQ_SENT"));
            return crow::response(409, tr->get("ERR_REQ_EXIST"));
        });

        // ---------------------------------------------------------
        // ROTA 8.2: LISTAR PENDENTES (Minhas Notificações)
        // ---------------------------------------------------------
        CROW_ROUTE(app, "/api/friends/pending")
        ([](const crow::request& req){
            int myId = Router::authenticate(req);
            if (myId == -1) return crow::json::wvalue();

            Auth::User me;
            me.setId(myId);
            std::vector<Auth::User> list = me.getPendingRequests();
            return crow::json::wvalue({}); // (Placeholder para o loop de JSON)
        });

        // ---------------------------------------------------------
        // ROTA 8.3: RESPONDER (Aceitar/Recusar)
        // ---------------------------------------------------------
        CROW_ROUTE(app, "/api/friends/respond").methods(crow::HTTPMethod::Post)
        ([](const crow::request& req){
            auto* tr = Core::Translation::getInstance();
            int myId = Router::authenticate(req); // <--- BLINDADO
            if (myId == -1) return crow::response(401, tr->get("ERR_AUTH_FAILED"));

            auto x = crow::json::load(req.body);
            if (!x || !x.has("requester_id") || !x.has("action")) {
                return crow::response(400, tr->get("ERR_MISSING"));
            }
            
            Auth::User me, requester;
            me.setId(myId); 
            requester.setId(x["requester_id"].i());
            std::string action = x["action"].s();

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
            int userId = Router::authenticate(req);
            auto* tr = Core::Translation::getInstance();
            if (userId == -1) return crow::response(401);

            auto x = crow::json::load(req.body);
            if (!x || !x.has("post_id") || !x.has("content")) return crow::response(400);

            Content::Comment c;
            c.setPostId(x["post_id"].i());
            c.setAuthorId(userId); // <--- Usa o ID do token
            c.setContent(x["content"].s());
            if (x.has("parent_id")) c.setParentId(x["parent_id"].i());

            if (c.save()) return crow::response(201, tr->get("MSG_COMMENT_ADDED"));
            return crow::response(500, tr->get("SQL_ERROR"));
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
            int userId = Router::authenticate(req);
            if (userId == -1) return crow::response(401);

            auto x = crow::json::load(req.body);
            if (!x || !x.has("post_id")) return crow::response(400);

            Content::Like like;
            like.setPostId(x["post_id"].i());
            like.setUserId(userId); // <--- Usa o ID do token

            return like.toggle() ? crow::response(201, tr->get("MSG_LIKE_ADDED")) 
                                 : crow::response(200, tr->get("MSG_LIKE_REMOVED"));
        });

        // ---------------------------------------------------------
        // ROTA 12: CENTRAL DE NOTIFICAÇÕES
        // GET /api/notifications/<my_id>
        // ---------------------------------------------------------
        CROW_ROUTE(app, "/api/notifications")
        ([](const crow::request& req){
            int userId = Router::authenticate(req);
            if (userId == -1) return crow::json::wvalue();
            return crow::json::wvalue(Content::Notification::getByUser(userId));
        });

        // ---------------------------------------------------------
        // ROTA 13: MARCAR NOTIFICAÇÕES COMO LIDAS
        // POST /api/notifications/read
        // JSON: { "user_id": 1 }
        // ---------------------------------------------------------
        CROW_ROUTE(app, "/api/notifications/read").methods(crow::HTTPMethod::Post)
        ([](const crow::request& req){
            int userId = Router::authenticate(req); // <--- BLINDADO
            if (userId == -1) return crow::response(401);
            
            Content::Notification::markAllAsRead(userId); // Usa ID do Token
            return crow::response(200, "OK");
        });

        // ROTA 14: LOG DE ATIVIDADES
        CROW_ROUTE(app, "/api/logs/<int>")
        ([](int userId){
            // Agora chamamos a classe correta
            auto logs = Core::Logger::getUserLogs(userId);
            return crow::json::wvalue(logs);
        });

        // ---------------------------------------------------------
        // ROTA 15: CRIAR COMUNIDADE 
        // ---------------------------------------------------------
        CROW_ROUTE(app, "/api/communities").methods(crow::HTTPMethod::Post)
        ([](const crow::request& req){
            auto* tr = Core::Translation::getInstance();
            int userId = Router::authenticate(req);
            
            if (userId == -1) return crow::response(401, tr->get("ERR_AUTH_FAILED"));

            // VERIFICAÇÃO EXTRA DE E-MAIL
            Auth::User user;
            if (Auth::User::findById(userId, user)) {
                if (!user.getVerified()) {
                    // Agora usa a chave de tradução!
                    return crow::response(403, tr->get("ERR_NOT_VERIFIED")); 
                }
            }

            auto x = crow::json::load(req.body);
            if (!x || !x.has("name")) return crow::response(400, tr->get("ERR_MISSING"));

            Social::Community comm;
            comm.setName(x["name"].s());
            comm.setOwnerId(userId);
            if (x.has("description")) comm.setDescription(x["description"].s());
            
            if (comm.save()) return crow::response(201, tr->get("MSG_COMM_CREATED"));
            return crow::response(500, tr->get("SQL_ERROR"));
        });

        // ---------------------------------------------------------
        // ROTA 15.2: SOLICITAR ENTRADA
        // ---------------------------------------------------------
        CROW_ROUTE(app, "/api/communities/request").methods(crow::HTTPMethod::Post)
        ([](const crow::request& req){
            auto* tr = Core::Translation::getInstance();
            int userId = Router::authenticate(req); 
            if (userId == -1) return crow::response(401);

            auto x = crow::json::load(req.body);
            if (!x || !x.has("community_id")) return crow::response(400);

            Social::Community comm;
            comm.setId(x["community_id"].i());

            if (comm.requestJoin(userId)) return crow::response(200, tr->get("MSG_REQ_SENT"));
            return crow::response(400, tr->get("ERR_REQ_EXIST"));
        });

        CROW_ROUTE(app, "/api/communities/approve").methods(crow::HTTPMethod::Post)
        ([](const crow::request& req){
            auto* tr = Core::Translation::getInstance();
            int adminId = Router::authenticate(req); 
            if (adminId == -1) return crow::response(401, tr->get("ERR_AUTH_FAILED"));
            
            auto x = crow::json::load(req.body);
            if (!x || !x.has("community_id") || !x.has("user_id") || !x.has("admin_id")) {
                return crow::response(400, tr->get("ERR_MISSING"));
            }

            int commId = x["community_id"].i();
            int targetUserId = x["user_id"].i();
            int adminId = x["admin_id"].i(); // Quem está tentando aprovar

            if (!Social::Community::checkPermission(x["community_id"].i(), adminId, Social::CommunityRole::ADMIN)) {
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
            int adminId = Router::authenticate(req);
            if (adminId == -1) return crow::response(401);

            auto x = crow::json::load(req.body);
            if (!x || !x.has("community_id")) return crow::response(400);

            Social::Community comm;
            comm.setId(x["community_id"].i());

            if (comm.destroy(adminId)) return crow::response(200, tr->get("MSG_COMM_DELETED"));
            return crow::response(403, tr->get("ERR_NOT_MASTER"));
        });

        // ROTA 16: GERENCIAR ROLES (Promover/Rebaixar)
        CROW_ROUTE(app, "/api/communities/role").methods(crow::HTTPMethod::Post)
        ([](const crow::request& req){
            auto* tr = Core::Translation::getInstance();
            int masterAdminId = Router::authenticate(req); 
            if (masterAdminId == -1) return crow::response(401, tr->get("ERR_AUTH_FAILED"));
            auto x = crow::json::load(req.body);
            
            int commId = x["community_id"].i();
            int masterId = masterAdminId; 
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
            int adminId = Router::authenticate(req);
            if (adminId == -1) return crow::response(401, tr->get("ERR_AUTH_FAILED"));

            auto x = crow::json::load(req.body);
            
            if (!x.has("community_id") || !x.has("admin_id") || !x.has("target_id")) {
                return crow::response(400, tr->get("ERR_MISSING"));
            }

            Social::Community comm;
            comm.setId(x["community_id"].i());

            if (comm.removeMember(x["target_id"].i(), adminId)) {
                return crow::response(200, tr->get("MSG_MEMBER_REMOVED"));
            }
            
            return crow::response(403, tr->get("ERR_PERMISSION_DENIED"));
        });

        // ROTA 17: TIMELINE DA COMUNIDADE
        CROW_ROUTE(app, "/api/communities/<int>/posts")
        ([](const crow::request& req, int commId){
            auto* tr = Core::Translation::getInstance();
            auto* db = Core::Database::getInstance();
            std::vector<crow::json::wvalue> postsList;

            char* viewerParam = req.url_params.get("viewer");
            int viewerId = viewerParam ? std::stoi(viewerParam) : -1;

            bool isPrivate = false;
            db->query("SELECT is_private FROM communities WHERE id = " + std::to_string(commId), [&](int argc, char** argv, char**){
                isPrivate = (std::stoi(argv[0]) == 1);
                return 0;
            });

            if (isPrivate) {
                if (!Social::Community::isMember(commId, viewerId)) {
                    return crow::response(403, tr->get("ERR_PRIVATE_COMMUNITY"));
                }
            }

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
            return crow::response(crow::json::wvalue(postsList));
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

        // ---------------------------------------------------------
        // ROTA 19: SAIR DE UMA COMUNIDADE (CORRIGIDA 🛡️)
        // ---------------------------------------------------------
        CROW_ROUTE(app, "/api/communities/leave").methods(crow::HTTPMethod::Post)
        ([](const crow::request& req){
            auto* tr = Core::Translation::getInstance();
            int userId = Router::authenticate(req); // <--- O ID vem do Token!
            if (userId == -1) return crow::response(401, tr->get("ERR_AUTH_FAILED"));

            auto x = crow::json::load(req.body);
            if (!x || !x.has("community_id")) return crow::response(400, tr->get("ERR_MISSING"));
            
            int commId = x["community_id"].i();

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
            int currentMasterId = Router::authenticate(req); // <--- O Dono é quem manda a req
            if (currentMasterId == -1) return crow::response(401, tr->get("ERR_AUTH_FAILED"));

            auto x = crow::json::load(req.body);
            if (!x || !x.has("community_id") || !x.has("new_master")) {
                return crow::response(400, tr->get("ERR_MISSING"));
            }

            Social::Community comm;
            comm.setId(x["community_id"].i());

            if (comm.transferOwnership(currentMasterId, x["new_master"].i())) {
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

        // ---------------------------------------------------------
        // ROTA 23: PROCESSAR PEDIDO DE ENTRADA (CORRIGIDA 🛡️)
        // ---------------------------------------------------------
        CROW_ROUTE(app, "/api/communities/respond_request").methods(crow::HTTPMethod::Post)
        ([](const crow::request& req){
            auto* tr = Core::Translation::getInstance();
            int adminId = Router::authenticate(req); // <--- Quem está aprovando?
            if (adminId == -1) return crow::response(401, tr->get("ERR_AUTH_FAILED"));

            auto x = crow::json::load(req.body);
            if (!x || !x.has("community_id") || !x.has("user_id") || !x.has("action")) {
                return crow::response(400, tr->get("ERR_MISSING"));
            }

            int commId = x["community_id"].i();
            int targetUserId = x["user_id"].i();
            std::string action = x["action"].s(); 

            if (!Social::Community::checkPermission(commId, adminId, Social::CommunityRole::ADMIN)) {
                return crow::response(403, tr->get("ERR_PERMISSION_DENIED"));
            }

            auto* db = Core::Database::getInstance();

            if (action == "accept") {
                if (Social::Community::addMember(commId, targetUserId, Social::CommunityRole::MEMBER)) {
                    db->execute("DELETE FROM community_requests WHERE community_id = " + std::to_string(commId) + 
                                " AND user_id = " + std::to_string(targetUserId));
                    return crow::response(200, tr->get("MSG_REQ_ACCEPTED"));
                }
            } else if (action == "reject") {
                db->execute("DELETE FROM community_requests WHERE community_id = " + std::to_string(commId) + 
                            " AND user_id = " + std::to_string(targetUserId));
                return crow::response(200, tr->get("MSG_REQ_REJECTED"));
            }

            return crow::response(400, tr->get("ERR_PROCESS_REQ"));
        });

        // ---------------------------------------------------------
        // ROTA 24: SUGESTÕES DE CONTEUDO
        // ---------------------------------------------------------
        CROW_ROUTE(app, "/api/discover/<int>")
        ([](int userId){
            auto* db = Core::Database::getInstance();
            std::vector<crow::json::wvalue> suggestions;

            // Query de Descoberta:
            // 1. Pega posts de perfis PÚBLICOS
            // 2. Que não sejam o próprio usuário
            // 3. Que NÃO sejam amigos (para sugerir gente nova)
            std::string sql = 
                "SELECT p.id, p.content, u.username, "
                "(SELECT COUNT(*) FROM likes l WHERE l.post_id = p.id) as total_likes "
                "FROM posts p "
                "JOIN users u ON p.author_id = u.id "
                "WHERE u.is_private = 0 "
                "AND u.id != " + std::to_string(userId) + " "
                "AND u.id NOT IN (SELECT user_id_1 FROM friendships WHERE user_id_2 = " + std::to_string(userId) + " "
                "                 UNION SELECT user_id_2 FROM friendships WHERE user_id_1 = " + std::to_string(userId) + ") "
                "ORDER BY total_likes DESC LIMIT 20;";

            db->query(sql, [&](int argc, char** argv, char**) {
                crow::json::wvalue item;
                item["post_id"] = std::stoi(argv[0]);
                item["content"] = argv[1];
                item["author"] = argv[2];
                item["likes"] = std::stoi(argv[3]);
                suggestions.push_back(std::move(item));
                return 0;
            });

            return crow::response(crow::json::wvalue(suggestions));
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
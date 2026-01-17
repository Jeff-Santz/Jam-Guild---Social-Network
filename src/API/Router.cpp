#include "API/Router.h"
#include "Auth/User.h"
#include "Content/Post.h"
#include "Content/Comment.h"
#include "Content/Like.h" 
#include "Content/Notification.h"
#include "Social/Community.h"
#include "Social/Report.h"
#include "Core/Translation.h"
#include "Core/Utils.h"
#include "Core/Database.h"
#include "Core/Logger.h"
#include "Core/TokenService.h"
#include "Core/Location.h"
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

    // Helper para adicionar CORS em qualquer resposta
    crow::response corsResponse(crow::response res) {
        res.add_header("Access-Control-Allow-Origin", "*");
        res.add_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
        res.add_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
        return res;
    }

    void Router::setupRoutes(crow::SimpleApp& app) {

        // ---------------------------------------------------------
        // ROTA: BAIXAR DICIONÁRIO DE TRADUÇÃO
        // O Frontend chama isso ao iniciar para traduzir a tela
        // ---------------------------------------------------------
        CROW_ROUTE(app, "/api/translations")
        ([](){
            auto* tr = Core::Translation::getInstance();
            // Pega o mapa inteiro (Key -> Valor)
            auto allTranslations = tr->getAll(); 
            
            crow::json::wvalue json;
            for (const auto& kv : allTranslations) {
                json[kv.first] = kv.second;
            }
            return corsResponse(crow::response(json));
        });
        
        // ---------------------------------------------------------
        // ROTA 1: HEALTH CHECK
        // ---------------------------------------------------------
        CROW_ROUTE(app, "/health")([](){
            auto* tr = Core::Translation::getInstance();
            crow::json::wvalue x;
            x["status"] = "online";
            // NÃO PRECISA MAIS DE corsResponse()!
            return crow::response(x); 
        });

        // ---------------------------------------------------------
        // ROTA 2: CADASTRO
        // ---------------------------------------------------------
        CROW_ROUTE(app, "/api/signup").methods(crow::HTTPMethod::Post)
        ([](const crow::request& req){
            auto* tr = Core::Translation::getInstance();
            auto x = crow::json::load(req.body);
            
            if (!x) return corsResponse(crow::response(400, tr->get("ERR_JSON")));
            Auth::User newUser;
            
            // Agora aceitamos se o email for gerado automaticamente pelo front
            if (!x.has("username") || !x.has("email") || !x.has("password")) {
                 return corsResponse(crow::response(400, tr->get("ERR_MISSING")));
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
                    return corsResponse(crow::response(400, errorMsg));
                }
            }

            if (x.has("is_private")) newUser.setPrivate(x["is_private"].b());

            if (x.has("city") && x.has("state")) {
                std::string c = x["city"].s();
                std::string s = x["state"].s();
                if (!Core::Location::isValid(c, s)) {
                    return corsResponse(crow::response(400, tr->get("ERR_INVALID_LOCATION")));
                }
                newUser.setCity(c);
                newUser.setState(s);
            }

            if (newUser.save()) {
                crow::json::wvalue res;
                res["message"] = tr->get("MSG_CREATED");
                res["id"] = newUser.getId();
                return corsResponse(crow::response(201, res));
            }
            return corsResponse(crow::response(409, tr->get("ERR_CONFLICT")));
        });

        // ---------------------------------------------------------
        // ROTA NOVA: PERFIL PÚBLICO (Traduzida & Blindada)
        // ---------------------------------------------------------
        CROW_ROUTE(app, "/api/users/<int>")
        ([](const crow::request& req, int targetId){
            // 1. Instância de Tradução
            auto* tr = Core::Translation::getInstance(); 
            int viewerId = Router::authenticate(req);
            auto* db = Core::Database::getInstance();
            crow::json::wvalue profile;

            // 2. Busca Dados
            bool found = false;
            bool isPrivate = false;
            std::string sqlUser = "SELECT username, bio, city, state, creation_date, is_verified, is_private FROM users WHERE id = " + std::to_string(targetId);
            
            db->query(sqlUser, [&](int, char** argv, char**){
                profile["id"] = targetId;
                profile["username"] = argv[0];
                profile["bio"] = argv[1] ? argv[1] : "";
                
                // USANDO O TR->GET() CORRETAMENTE
                if (argv[2] && argv[3] && std::string(argv[2]) != "") {
                    profile["location"] = std::string(argv[2]) + " - " + std::string(argv[3]);
                } else {
                    profile["location"] = tr->get("LBL_UNKNOWN_LOC"); // <--- Do Dicionário
                }

                profile["joined_at"] = argv[4] ? argv[4] : "";
                profile["is_verified"] = std::stoi(argv[5]);
                isPrivate = (std::stoi(argv[6]) == 1);
                found = true;
                return 0;
            });

            if (!found) return corsResponse(crow::response(404));

            // 3. Verifica Amizade
            std::string status = "none";
            if (viewerId != -1) {
                if (viewerId == targetId) {
                    status = "self";
                } else {
                    std::string sqlFriend = "SELECT count(*) FROM friendships WHERE ((user_id_1=" + std::to_string(viewerId) + 
                                            " AND user_id_2=" + std::to_string(targetId) + ") OR " +
                                            "(user_id_1=" + std::to_string(targetId) + " AND user_id_2=" + std::to_string(viewerId) + ")) AND status=1";
                    
                    db->query(sqlFriend, [&](int, char** argv, char**){
                        if (std::stoi(argv[0]) > 0) status = "friend";
                        return 0;
                    });

                    if (status == "none") {
                         db->query("SELECT count(*) FROM friendships WHERE user_id_1=" + std::to_string(viewerId) + 
                                  " AND user_id_2=" + std::to_string(targetId) + " AND status=0", [&](int, char** argv, char**){
                            if (std::stoi(argv[0]) > 0) status = "pending_sent";
                            return 0;
                        });
                        if (status == "none") {
                             db->query("SELECT count(*) FROM friendships WHERE user_id_1=" + std::to_string(targetId) + 
                                      " AND user_id_2=" + std::to_string(viewerId) + " AND status=0", [&](int, char** argv, char**){
                                if (std::stoi(argv[0]) > 0) status = "pending_received";
                                return 0;
                            });
                        }
                    }
                }
            }
            profile["friend_status"] = status;

            // 4. APLICA A CENSURA TRADUZIDA
            if (isPrivate && status != "friend" && status != "self") {
                profile["bio"] = tr->get("LBL_PRIVATE_BIO");     // <--- TRADUZIDO
                profile["location"] = tr->get("LBL_RESTRICTED"); // <--- TRADUZIDO
                profile["is_locked"] = true;
            } else {
                profile["is_locked"] = false;
            }

            return corsResponse(crow::response(profile));
        });

        // ---------------------------------------------------------
        // ROTA NOVA: TIMELINE DO USUÁRIO
        // ---------------------------------------------------------
        CROW_ROUTE(app, "/api/users/<int>/posts")
        ([](const crow::request& req, int targetId){
            // Não precisa traduzir nada aqui, mas mantemos o padrão
            int viewerId = Router::authenticate(req);
            auto* db = Core::Database::getInstance();
            
            bool canView = false;
            bool isPrivate = false;
            
            db->query("SELECT is_private FROM users WHERE id = " + std::to_string(targetId), [&](int, char** argv, char**){
                isPrivate = (std::stoi(argv[0]) == 1);
                return 0;
            });

            if (viewerId == targetId) canView = true;
            else if (!isPrivate) canView = true;
            else {
                std::string sqlF = "SELECT count(*) FROM friendships WHERE ((user_id_1=" + std::to_string(viewerId) + 
                                   " AND user_id_2=" + std::to_string(targetId) + ") OR " +
                                   "(user_id_1=" + std::to_string(targetId) + " AND user_id_2=" + std::to_string(viewerId) + ")) AND status=1";
                db->query(sqlF, [&](int, char** argv, char**){
                    if (std::stoi(argv[0]) > 0) canView = true;
                    return 0;
                });
            }

            std::vector<crow::json::wvalue> postsList;
            if (!canView) return corsResponse(crow::response(crow::json::wvalue(postsList)));

            // Query otimizada com COALESCE para tratar NULLs
            std::string sql = "SELECT p.id, p.content, u.username, COALESCE(c.name, 'Pessoal') as origin, "
                              "(SELECT COUNT(*) FROM likes l WHERE l.post_id = p.id) as likes_count, "
                              "p.media_url, p.media_type "
                              "FROM posts p JOIN users u ON p.author_id = u.id "
                              "LEFT JOIN communities c ON p.community_id = c.id "
                              "WHERE p.author_id = " + std::to_string(targetId) + 
                              " ORDER BY p.id DESC LIMIT 50;";

             db->query(sql, [&](int argc, char** argv, char**) {
                crow::json::wvalue p;
                p["id"] = std::stoi(argv[0]);
                p["content"] = argv[1];
                p["author"] = argv[2];
                p["origin"] = argv[3];
                p["likes"] = std::stoi(argv[4]);
                p["media_url"] = argv[5] ? argv[5] : "";
                p["media_type"] = argv[6] ? argv[6] : "";
                postsList.push_back(std::move(p));
                return 0;
            });

            return corsResponse(crow::response(crow::json::wvalue(postsList)));
        });

        // ---------------------------------------------------------
        // ROTA AUXILIAR: SIMULAR VALIDAÇÃO DE EMAIL
        // ---------------------------------------------------------
        CROW_ROUTE(app, "/api/verify_me").methods(crow::HTTPMethod::Post)
        ([](const crow::request& req){
            auto* tr = Core::Translation::getInstance(); 
            
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

            if (!x) return corsResponse(crow::response(400, tr->get("ERR_JSON")));

            std::string identifier = x["identifier"].s();
            std::string password = x["password"].s();

            Auth::User user;
            if (user.login(identifier, password)) {
                std::string token = Core::TokenService::createToken(user.getId());
                crow::json::wvalue res;
                res["status"] = "success";
                res["token"] = token;
                res["username"] = user.getUsername();
                res["id"] = user.getId();
                
                // Retorna com header CORS
                return corsResponse(crow::response(200, res));
            } else {
                return corsResponse(crow::response(401, tr->get("ERR_AUTH_FAILED")));
            }
        });

        // ---------------------------------------------------------
        // ROTA 4: CRIAR POST (Atualizada para suportar comunidades e Arquivos de Midia)
        // ---------------------------------------------------------
        CROW_ROUTE(app, "/api/posts").methods(crow::HTTPMethod::Post)
        ([](const crow::request& req){
            auto* tr = Core::Translation::getInstance();
            int userId = Router::authenticate(req);
            
            // Se falhar auth, retorna com CORS também senão o front não vê o erro 401
            if (userId == -1) return corsResponse(crow::response(401, tr->get("ERR_AUTH_FAILED")));

            auto x = crow::json::load(req.body);
            if (!x || !x.has("content")) return corsResponse(crow::response(400, tr->get("ERR_MISSING")));

            Content::Post newPost;
            newPost.setAuthorId(userId); 
            newPost.setContent(x["content"].s());
            if (x.has("community_id")) newPost.setCommunityId(x["community_id"].i());
            if (x.has("tags")) newPost.setTags(x["tags"].s());

            if (newPost.save()) {
                // MÍDIA
                if (x.has("media_base64")) {
                    std::string b64 = x["media_base64"].s();
                    if (!b64.empty()) {
                        std::string path = Core::Utils::saveBase64Image(b64, newPost.getId());
                        
                        if (path.empty()) {
                             return corsResponse(crow::response(400, tr->get("ERR_FILE_TOO_LARGE")));
                        }

                        auto* db = Core::Database::getInstance();
                        std::string sql = "UPDATE posts SET media_url = '" + path + "', media_type = 'image' WHERE id = " + std::to_string(newPost.getId());
                        db->execute(sql);
                    }
                }
                return corsResponse(crow::response(201, tr->get("MSG_POST_CREATED")));
            }
            return corsResponse(crow::response(500, tr->get("SQL_ERROR")));
        });

        // ---------------------------------------------------------
        // ROTA 5: FEED GEO-LOCALIZADO + MÍDIA (Versão Final)
        // ---------------------------------------------------------
        CROW_ROUTE(app, "/api/feed")
        ([](const crow::request& req) {
            int userId = Router::authenticate(req);
            if (userId == -1) return corsResponse(crow::response(401));
            
            auto* db = Core::Database::getInstance();
            char* viewerParam = req.url_params.get("viewer");
            int viewerId = viewerParam ? std::stoi(viewerParam) : userId;

            std::string myCity = "";
            std::string myState = "";
            db->query("SELECT city, state FROM users WHERE id = " + std::to_string(viewerId), 
            [&](int, char** argv, char**){
                myCity = argv[0] ? argv[0] : "";
                myState = argv[1] ? argv[1] : "";
                return 0;
            });

            std::string sql = 
                "SELECT p.id, p.content, u.username, COALESCE(c.name, 'Pessoal') as origin, "
                "(SELECT COUNT(*) FROM likes l WHERE l.post_id = p.id) as likes_count, "
                "u.city, u.state, p.media_url, p.media_type, "
                "CASE "
                "   WHEN u.id IN (SELECT user_id_1 FROM friendships WHERE user_id_2 = " + std::to_string(viewerId) + " AND status = 1 "
                "                UNION SELECT user_id_2 FROM friendships WHERE user_id_1 = " + std::to_string(viewerId) + " AND status = 1) THEN 50 " 
                "   WHEN p.community_id IN (SELECT community_id FROM community_members WHERE user_id = " + std::to_string(viewerId) + ") THEN 50 " 
                "   WHEN u.city = '" + myCity + "' AND u.city != '' THEN 30 "
                "   WHEN u.state = '" + myState + "' AND u.state != '' THEN 10 "
                "   ELSE 0 "
                "END + "
                "((SELECT COUNT(*) FROM likes l WHERE l.post_id = p.id) * 2) " 
                "as algorithm_score "
                "FROM posts p "
                "JOIN users u ON p.author_id = u.id "
                "LEFT JOIN communities c ON p.community_id = c.id "
                "WHERE (u.is_private = 0 OR u.id = " + std::to_string(viewerId) + 
                " OR u.id IN (SELECT user_id_1 FROM friendships WHERE user_id_2 = " + std::to_string(viewerId) + " AND status = 1)) "
                "ORDER BY algorithm_score DESC LIMIT 50;"; 

            std::vector<crow::json::wvalue> feedList;
            db->query(sql, [&](int argc, char** argv, char**) {
                crow::json::wvalue p;
                p["id"] = std::stoi(argv[0]);
                p["content"] = argv[1];
                p["author"] = argv[2];
                p["origin"] = argv[3];
                p["likes"] = std::stoi(argv[4]);
                p["location"] = (argv[5] && argv[6]) ? (std::string(argv[5]) + " - " + std::string(argv[6])) : "Global";
                p["media_url"] = argv[7] ? argv[7] : "";
                p["media_type"] = argv[8] ? argv[8] : "";
                feedList.push_back(std::move(p));
                return 0;
            });

            return corsResponse(crow::response(crow::json::wvalue(feedList)));
        });

        // ---------------------------------------------------------
        // ROTA 6: EDITAR PERFIL (COM A TRAVA DE E-MAIL 🛡️)
        // ---------------------------------------------------------
        CROW_ROUTE(app, "/api/profile").methods(crow::HTTPMethod::Put)
        ([](const crow::request& req){
            auto* tr = Core::Translation::getInstance();
            int userId = Router::authenticate(req);
            if (userId == -1) return corsResponse(crow::response(401));

            auto x = crow::json::load(req.body);
            Auth::User user;
            if (!Auth::User::findById(userId, user)) return corsResponse(crow::response(404));

            // 1. Fotos (Avatar e Capa)
            if (x.has("avatar_base64")) {
                std::string path = Core::Utils::saveBase64Image(x["avatar_base64"].s(), userId); 
                // Nota: saveBase64Image usa ID do post ou user. Vamos assumir que funciona.
                // Idealmente, crie uma pasta /avatars, mas salvar na mesma lógica funciona.
                user.setAvatarUrl(path); // *VOCÊ PRECISA ADICIONAR setAvatarUrl no User.h*
                // Se não tiver o método na classe User, faça update manual no DB:
                Core::Database::getInstance()->execute("UPDATE users SET avatar_url = '" + path + "' WHERE id = " + std::to_string(userId));
            }
            if (x.has("cover_base64")) {
                std::string path = Core::Utils::saveBase64Image(x["cover_base64"].s(), userId);
                Core::Database::getInstance()->execute("UPDATE users SET cover_url = '" + path + "' WHERE id = " + std::to_string(userId));
            }

            // 2. Dados (Update Inteligente)
            if (x.has("bio")) user.setBio(x["bio"].s());
            if (x.has("is_private")) user.setPrivate(x["is_private"].b());
            
            // Só muda e-mail se vier algo diferente e válido
            if (x.has("email")) {
                std::string em = x["email"].s();
                if (!em.empty() && em != user.getEmail()) user.setEmail(em);
            }
            
            // Só muda data se vier
            if (x.has("birth_date")) {
                try { user.setBirthDate(Core::Utils::validateISO(x["birth_date"].s())); } catch(...){}
            }

            if (user.update()) return corsResponse(crow::response(200, tr->get("MSG_PROFILE_UPDATED")));
            return corsResponse(crow::response(500));
        });

        // ---------------------------------------------------------
        // ROTA 7: BUSCAR USUÁRIOS
        // ---------------------------------------------------------
        CROW_ROUTE(app, "/api/search")
        ([](const crow::request& req){
            char* queryParam = req.url_params.get("q");
            if (!queryParam) return corsResponse(crow::response(crow::json::wvalue(std::vector<crow::json::wvalue>{})));

            std::vector<Auth::User> results = Auth::User::search(std::string(queryParam));
            std::vector<crow::json::wvalue> jsonList;
            for (const auto& u : results) {
                crow::json::wvalue item;
                item["id"] = u.getId();
                item["username"] = u.getUsername();
                item["bio"] = u.getBio();
                jsonList.push_back(item);
            }
            // Envolvendo no corsResponse
            return corsResponse(crow::response(crow::json::wvalue(jsonList)));
        });

        // ---------------------------------------------------------
        // ROTA 8.1: ENVIAR SOLICITAÇÃO (Friend Request)
        // ---------------------------------------------------------
        CROW_ROUTE(app, "/api/friends/request").methods(crow::HTTPMethod::Post)
        ([](const crow::request& req){
            int myId = Router::authenticate(req);
            auto* tr = Core::Translation::getInstance();
            if (myId == -1) return corsResponse(crow::response(401));

            auto x = crow::json::load(req.body);
            if (!x || !x.has("to_id")) return corsResponse(crow::response(400));

            Auth::User me, other;
            me.setId(myId);
            other.setId(x["to_id"].i());

            if (me.sendFriendRequest(&other)) return corsResponse(crow::response(200, tr->get("MSG_REQ_SENT")));
            return corsResponse(crow::response(409, tr->get("ERR_REQ_EXIST")));
        });

        // ---------------------------------------------------------
        // ROTA 8.2: LISTAR PENDENTES (Minhas Notificações)
        // ---------------------------------------------------------
        CROW_ROUTE(app, "/api/friends/pending")
        ([](const crow::request& req){
            int myId = Router::authenticate(req);
            if (myId == -1) return corsResponse(crow::response(crow::json::wvalue(std::vector<crow::json::wvalue>{})));

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
            return corsResponse(crow::response(crow::json::wvalue(jsonList)));
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
                return corsResponse(crow::response(200, tr->get("MSG_REQ_PROCESSED")));
            } else {
                return corsResponse(crow::response(500, tr->get("ERR_REQ_PROCESS")));
            }
        });

        // ---------------------------------------------------------
        // ROTA 9: COMENTAR
        // ---------------------------------------------------------
        CROW_ROUTE(app, "/api/comments").methods(crow::HTTPMethod::Post)
        ([](const crow::request& req){
            int userId = Router::authenticate(req);
            auto* tr = Core::Translation::getInstance();
            if (userId == -1) return corsResponse(crow::response(401));

            auto x = crow::json::load(req.body);
            if (!x || !x.has("post_id") || !x.has("content")) return corsResponse(crow::response(400));

            Content::Comment c;
            c.setPostId(x["post_id"].i());
            c.setAuthorId(userId);
            c.setContent(x["content"].s());
            if (x.has("parent_id")) c.setParentId(x["parent_id"].i());

            if (c.save()) return corsResponse(crow::response(201, tr->get("MSG_COMMENT_ADDED")));
            return corsResponse(crow::response(500, tr->get("SQL_ERROR")));
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

            return corsResponse(crow::response(crow::json::wvalue(jsonList)));
        });

        // ---------------------------------------------------------
        // ROTA 11: DAR/TIRAR LIKE (Toggle)
        // ---------------------------------------------------------
        CROW_ROUTE(app, "/api/likes").methods(crow::HTTPMethod::Post)
        ([](const crow::request& req){
            auto* tr = Core::Translation::getInstance();
            int userId = Router::authenticate(req);
            if (userId == -1) return corsResponse(crow::response(401));

            auto x = crow::json::load(req.body);
            if (!x || !x.has("post_id")) return corsResponse(crow::response(400));

            Content::Like like;
            like.setPostId(x["post_id"].i());
            like.setUserId(userId);

            return corsResponse(like.toggle() ? crow::response(201, tr->get("MSG_LIKE_ADDED")) 
                                              : crow::response(200, tr->get("MSG_LIKE_REMOVED")));
        });

        // ---------------------------------------------------------
        // ROTA 12: CENTRAL DE NOTIFICAÇÕES
        // GET /api/notifications/<my_id>
        // ---------------------------------------------------------
        CROW_ROUTE(app, "/api/notifications")
        ([](const crow::request& req){
            auto* tr = Core::Translation::getInstance();
            int userId = Router::authenticate(req);
            if (userId == -1) return corsResponse(crow::response(401, tr->get("ERR_AUTH_FAILED")));
            auto notifs = Content::Notification::getByUser(userId);
            return corsResponse(crow::response(crow::json::wvalue(notifs)));
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
            return corsResponse(crow::response(200, "OK"));
        });

        // ROTA 14: LOG DE ATIVIDADES
        CROW_ROUTE(app, "/api/logs/<int>")
        ([](int userId){
            // Agora chamamos a classe correta
            auto logs = Core::Logger::getUserLogs(userId);
            return crow::json::wvalue(logs);
        });

        // ---------------------------------------------------------
        // ROTA 14.5: LISTAR TODAS AS COMUNIDADES (Para o Frontend mostrar)
        // ---------------------------------------------------------
        CROW_ROUTE(app, "/api/communities")
        ([](){
            auto* db = Core::Database::getInstance();
            std::vector<crow::json::wvalue> comms;
            
            // Busca ID, Nome, Descrição e contagem de membros
            std::string sql = "SELECT c.id, c.name, c.description, c.city, c.state, "
                              "(SELECT COUNT(*) FROM community_members cm WHERE cm.community_id = c.id) as members "
                              "FROM communities c ORDER BY members DESC LIMIT 20;";
                              
            db->query(sql, [&](int, char** argv, char**){
                crow::json::wvalue c;
                c["id"] = std::stoi(argv[0]);
                c["name"] = argv[1];
                c["description"] = argv[2] ? argv[2] : "";
                c["location"] = (argv[3] && argv[4]) ? (std::string(argv[3]) + " - " + std::string(argv[4])) : "Global";
                c["members"] = std::stoi(argv[5]);
                comms.push_back(std::move(c));
                return 0;
            });
            
            return corsResponse(crow::response(crow::json::wvalue(comms)));
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
                    return corsResponse(crow::response(403, tr->get("ERR_NOT_VERIFIED"))); 
                }
            }

            auto x = crow::json::load(req.body);
            if (!x || !x.has("name")) return crow::response(400, tr->get("ERR_MISSING"));

            Social::Community comm;
            comm.setName(x["name"].s());
            comm.setOwnerId(userId);
            if (x.has("description")) comm.setDescription(x["description"].s());

            // VALIDAÇÃO GEOGRÁFICA (PADRONIZADA 🛡️)
            if (x.has("city") && x.has("state")) {
                std::string c = x["city"].s();
                std::string s = x["state"].s();
                
                if (!Core::Location::isValid(c, s)) {
                    return corsResponse(crow::response(400, tr->get("ERR_INVALID_LOCATION")));
                }
                comm.setCity(c);
                comm.setState(s);
            }
            
            if (comm.save()) return crow::response(201, tr->get("MSG_COMM_CREATED"));
            return corsResponse(crow::response(500, tr->get("SQL_ERROR")));
        });

        // ---------------------------------------------------------
        // ROTA 15.2: SOLICITAR ENTRADA
        // ---------------------------------------------------------
        CROW_ROUTE(app, "/api/communities/request").methods(crow::HTTPMethod::Post)
        ([](const crow::request& req){
            auto* tr = Core::Translation::getInstance();
            int userId = Router::authenticate(req); 
            if (userId == -1) return corsResponse(crow::response(401));

            auto x = crow::json::load(req.body);
            if (!x || !x.has("community_id")) return corsResponse(crow::response(400));
            int commId = x["community_id"].i();

            auto* db = Core::Database::getInstance();
            bool isPrivate = false;

            // 1. Verifica se é privada
            db->query("SELECT is_private FROM communities WHERE id = " + std::to_string(commId), [&](int, char** argv, char**){
                isPrivate = (std::stoi(argv[0]) == 1);
                return 0;
            });

            // 2. Se for PÚBLICA, entra direto!
            if (!isPrivate) {
                if (Social::Community::addMember(commId, userId, Social::CommunityRole::MEMBER)) {
                    return corsResponse(crow::response(200, "JOINED_DIRECTLY")); // Front vai ler isso
                }
                return corsResponse(crow::response(400, tr->get("ERR_ALREADY_MEMBER")));
            }

            // 3. Se for PRIVADA, cria pedido (Lógica antiga)
            Social::Community comm; comm.setId(commId);
            if (comm.requestJoin(userId)) return corsResponse(crow::response(200, "REQUEST_SENT"));
            
            return corsResponse(crow::response(409, tr->get("ERR_REQ_EXIST")));
        });

        CROW_ROUTE(app, "/api/communities/approve").methods(crow::HTTPMethod::Post)
        ([](const crow::request& req){
            auto* tr = Core::Translation::getInstance();
            int adminId = Router::authenticate(req); 
            if (adminId == -1) return corsResponse(crow::response(401, tr->get("ERR_AUTH_FAILED")));
            
            auto x = crow::json::load(req.body);
            if (!x || !x.has("community_id") || !x.has("user_id") || !x.has("admin_id")) {
                return corsResponse(crow::response(400, tr->get("ERR_MISSING")));
            }

            int commId = x["community_id"].i();
            int targetUserId = x["user_id"].i();

            if (!Social::Community::checkPermission(x["community_id"].i(), adminId, Social::CommunityRole::ADMIN)) {
                return corsResponse(crow::response(403, tr->get("ERR_PERMISSION_DENIED")));
            }

            if (Social::Community::addMember(commId, targetUserId, Social::CommunityRole::MEMBER)) {
                auto* db = Core::Database::getInstance();
                
                db->execute("DELETE FROM community_requests WHERE community_id = " + std::to_string(commId) + 
                            " AND user_id = " + std::to_string(targetUserId));
                
                return corsResponse(crow::response(200, tr->get("APROVE_COMM_MEMBER")));
            } 

            return corsResponse(crow::response(400, tr->get("ERR_ALREADY_MEMBER")));
        });

        // ROTA 15.1: EXCLUIR COMUNIDADE (DELETE)
        // Apenas o Master Admin pode destruir a página
        CROW_ROUTE(app, "/api/communities").methods(crow::HTTPMethod::Delete)
        ([](const crow::request& req){
            auto* tr = Core::Translation::getInstance();
            int adminId = Router::authenticate(req);
            if (adminId == -1) return crow::response(401);

            auto x = crow::json::load(req.body);
            if (!x || !x.has("community_id")) return corsResponse(crow::response(400));

            Social::Community comm;
            comm.setId(x["community_id"].i());

            if (comm.destroy(adminId)) return crow::response(200, tr->get("MSG_COMM_DELETED"));
            return corsResponse(crow::response(403, tr->get("ERR_NOT_MASTER")));
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

            std::string sql = "SELECT p.id, p.author_id, p.content, p.creation_date, u.username, "
                              "p.media_url, p.media_type, "
                              "(SELECT COUNT(*) FROM likes l WHERE l.post_id = p.id) as likes_count "
                              "FROM posts p JOIN users u ON p.author_id = u.id "
                              "WHERE p.community_id = " + std::to_string(commId) + 
                              " ORDER BY p.id DESC;";

             db->query(sql, [&](int argc, char** argv, char**) {
                crow::json::wvalue p;
                p["id"] = std::stoi(argv[0]);
                p["author_id"] = std::stoi(argv[1]);
                p["content"] = argv[2] ? argv[2] : "";
                p["date"] = argv[3] ? argv[3] : "";
                p["author"] = argv[4] ? argv[4] : "Unknown";
                // AQUI ESTAVA O ERRO: Faltava mapear a mídia
                p["media_url"] = argv[5] ? argv[5] : "";
                p["media_type"] = argv[6] ? argv[6] : "";
                p["likes"] = std::stoi(argv[7]);
                
                postsList.push_back(std::move(p));
                return 0;
            });

            return corsResponse(crow::response(crow::json::wvalue(postsList)));
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
            if (userId == -1) return corsResponse(crow::response(401, tr->get("ERR_AUTH_FAILED")));

            auto x = crow::json::load(req.body);
            if (!x || !x.has("community_id")) return crow::response(400, tr->get("ERR_MISSING"));
            
            int commId = x["community_id"].i();

            Social::Community comm;
            comm.setId(commId);

            // TRAVA: Master não pode sair sem transferir
            if (!comm.canLeave(userId)) {
                return corsResponse(crow::response(403, tr->get("ERR_MASTER_LEAVE")));
            }

            auto* db = Core::Database::getInstance();
            std::string sql = "DELETE FROM community_members WHERE community_id = " + 
                            std::to_string(commId) + " AND user_id = " + std::to_string(userId) + ";";
            
            if (db->execute(sql)) return corsResponse(crow::response(200, tr->get("MSG_LEAVE_COMM")));
            return corsResponse(crow::response(500, tr->get("ERR_LEAVE_COMM")));
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
        ([](const crow::request& req, int commId){
            int viewerId = Router::authenticate(req); // Precisamos saber quem tá vendo
            
            std::vector<Auth::User> members = Social::Community::getRichMembers(commId);
            std::vector<crow::json::wvalue> jsonList;
            auto* db = Core::Database::getInstance();

            for (const auto& u : members) {
                crow::json::wvalue item;
                item["id"] = u.getId();
                item["username"] = u.getUsername();
                item["role"] = Social::Community::getUserRole(commId, u.getId());
                
                // Verifica amizade
                bool isFriend = false;
                if (viewerId != -1 && viewerId != u.getId()) {
                     std::string sqlF = "SELECT count(*) FROM friendships WHERE ((user_id_1=" + std::to_string(viewerId) + 
                                       " AND user_id_2=" + std::to_string(u.getId()) + ") OR (user_id_1=" + std::to_string(u.getId()) + 
                                       " AND user_id_2=" + std::to_string(viewerId) + ")) AND status=1";
                     db->query(sqlF, [&](int, char** argv, char**){ if (std::stoi(argv[0]) > 0) isFriend = true; return 0; });
                }
                item["is_friend"] = isFriend;
                
                jsonList.push_back(std::move(item));
            }
            return corsResponse(crow::response(crow::json::wvalue(jsonList)));
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
                return corsResponse(crow::response(403, tr->get("ERR_PERMISSION_DENIED")));
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
                return corsResponse(crow::response(200, tr->get("MSG_REQ_REJECTED")));
            }

            return corsResponse(crow::response(400, tr->get("ERR_PROCESS_REQ")));
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

            return corsResponse(crow::response(crow::json::wvalue(suggestions)));
        });

        // ---------------------------------------------------------
        // ROTA 25: CRIAR DENÚNCIA (REPORT)
        // JSON: { "target_id": 5, "type": 2, "category": 1, "reason": "É golpe" }
        // ---------------------------------------------------------
        CROW_ROUTE(app, "/api/reports").methods(crow::HTTPMethod::Post)
        ([](const crow::request& req){
            auto* tr = Core::Translation::getInstance();
            int userId = Router::authenticate(req);
            if (userId == -1) return corsResponse(crow::response(401, tr->get("ERR_AUTH_FAILED")));

            auto x = crow::json::load(req.body);
            Social::Report r;
            r.setReporterId(userId);
            r.setTargetId(x["target_id"].i());
            r.setTargetType(x["type"].i());
            r.setCategory(x["category"].i());
            r.setReason(x["reason"].s());

            if (r.save()) return corsResponse(crow::response(201, tr->get("MSG_REPORT_CREATED")));
            return corsResponse(crow::response(500, tr->get("SQL_ERROR")));
        });

        // ---------------------------------------------------------
        // ROTA 26: DASHBOARD ADMIN (Mostra Categoria)
        // ---------------------------------------------------------
        CROW_ROUTE(app, "/api/admin/reports")
        ([](const crow::request& req){
            auto* tr = Core::Translation::getInstance();
            int userId = Router::authenticate(req);
            if (userId != 1) return corsResponse(crow::response(403, "Access Denied"));
            if (userId == -1) return corsResponse(crow::response(401)); 
            
            std::vector<Social::Report> list = Social::Report::getAllPending();
            std::vector<crow::json::wvalue> jsonList;
            
            for (const auto& r : list) {
                crow::json::wvalue item;
                item["id"] = r.getId();
                item["reporter_id"] = r.getReporterId();
                item["target_id"] = r.getTargetId();
                item["type"] = r.getTargetType();
                item["category"] = r.getCategory(); 
                item["reason"] = r.getReason();
                item["date"] = r.getCreationDate();
                jsonList.push_back(std::move(item));
            }
            return corsResponse(crow::response(crow::json::wvalue(jsonList)));
        });

        // ---------------------------------------------------------
        // ROTA 27: RESOLVER DENÚNCIA (Admin Action)
        // JSON: { "report_id": 10, "status": 1 } (1=Resolvido, 2=Ignorado)
        // ---------------------------------------------------------
        CROW_ROUTE(app, "/api/admin/reports/resolve").methods(crow::HTTPMethod::Post)
        ([](const crow::request& req){
            auto* tr = Core::Translation::getInstance();
            int userId = Router::authenticate(req);
            if (userId == -1) return crow::response(401);

            auto x = crow::json::load(req.body);
            if (!x || !x.has("report_id") || !x.has("status")) return crow::response(400);

            if (Social::Report::resolve(x["report_id"].i(), x["status"].i())) {
                return crow::response(200, tr->get("MSG_REPORT_RESOLVED"));
            }
            return crow::response(500, tr->get("SQL_ERROR"));
        });

        // ---------------------------------------------------------
        // ROTA 28: LISTAR CIDADES SUPORTADAS (Para dropdowns)
        // ---------------------------------------------------------
        CROW_ROUTE(app, "/api/locations")
        ([](){
            auto states = Core::Location::getStates();
            crow::json::wvalue jsonResponse;
            for (const auto& uf : states) {
                auto cities = Core::Location::getCities(uf);
                crow::json::wvalue cityList;
                for (size_t i = 0; i < cities.size(); i++) cityList[i] = cities[i];
                jsonResponse[uf] = std::move(cityList);
            }
            return corsResponse(crow::response(jsonResponse));
        });

        // ROTA MÁGICA: ATUALIZAR BANCO DE DADOS (Rode uma vez no navegador: /api/migrate)
        CROW_ROUTE(app, "/api/migrate")
        ([](){
            auto* db = Core::Database::getInstance();
            // Tenta adicionar as colunas. Se já existirem, vai dar erro mas não quebra.
            db->execute("ALTER TABLE users ADD COLUMN avatar_url TEXT DEFAULT '';");
            db->execute("ALTER TABLE users ADD COLUMN cover_url TEXT DEFAULT '';");
            return "Migracao concluida. Colunas de fotos criadas.";
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
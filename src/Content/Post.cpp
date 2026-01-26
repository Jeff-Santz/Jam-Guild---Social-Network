#include "Content/Post.h"
#include "Content/Notification.h"
#include "Core/Database.h"
#include "Core/Logger.h"
#include <iostream>

namespace Content {

    Post::Post() {
        this->authorId = -1;
        this->content = "";
        this->mediaUrl = "";
        this->mediaType = "";
    }

bool Post::save() {
    auto* db = Core::Database::getInstance();
    if (this->id == -1) {
        std::string commSql = (this->communityId == -1) ? "NULL" : std::to_string(this->communityId);

        // --- PROTEÇÃO ---
        std::string safeContent = Core::Database::escape(this->content);
        std::string safeTags = Core::Database::escape(this->tags);
        std::string safeMediaUrl = Core::Database::escape(this->mediaUrl); // Vai que o nome do arquivo tem aspas
        // ----------------

        std::string sql = "INSERT INTO posts (author_id, community_id, content, tags, media_url, media_type, creation_date) VALUES (" +
            std::to_string(this->authorId) + ", " + 
            commSql + ", '" + 
            safeContent + "', '" + 
            safeTags + "', '" +     
            safeMediaUrl + "', '" + 
            this->mediaType + "', '" +  
            this->creationDate + "');";

        if (db->execute(sql)) {
            this->id = db->getLastInsertId();
            
            // Log de auditoria
            Core::Logger::log(this->authorId, "NEW_POST", "Post ID: " + std::to_string(this->id));

            // --- LÓGICA DE NOTIFICAÇÃO ---
            if (this->communityId == -1) {
                // CASO A: Post Pessoal -> Notifica Amigos
                std::string sqlFriends = "SELECT user_id_1 FROM friendships WHERE user_id_2 = " + std::to_string(this->authorId) + " AND status = 1 "
                                         "UNION "
                                         "SELECT user_id_2 FROM friendships WHERE user_id_1 = " + std::to_string(this->authorId) + " AND status = 1;";
                
                db->query(sqlFriends, [&](int, char** argv, char**) {
                    int friendId = std::stoi(argv[0]);
                    Content::Notification::create(friendId, this->authorId, 5, this->id, "NOTIF_FRIEND_POST");
                    return 0;
                });
            } else {
                // CASO B: Post em Comunidade -> Notifica Membros
                std::string sqlMembers = "SELECT user_id FROM community_members WHERE community_id = " + 
                                         std::to_string(this->communityId) + " AND user_id != " + std::to_string(this->authorId) + ";";

                db->query(sqlMembers, [&](int, char** argv, char**) {
                    int memberId = std::stoi(argv[0]);
                    // Usamos um código de notificação específico para comunidades (ex: 6)
                    Content::Notification::create(memberId, this->authorId, 6, this->communityId, "NOTIF_COMMUNITY_NEW_POST");
                    return 0;
                });
            }

            return true;
        }
    }
    return false;
}

    std::vector<Post> Post::getPostsByUserId(int userId) {
        std::vector<Post> postsList;
        auto* db = Core::Database::getInstance();

        std::string sql = "SELECT id, author_id, content, creation_date FROM posts WHERE author_id = " 
                          + std::to_string(userId) + " ORDER BY id DESC;";

        auto callback = [&](int argc, char** argv, char** colNames) -> int {
            Post p;
            p.setId(std::stoi(argv[0]));
            p.setAuthorId(std::stoi(argv[1]));
            p.setContent(argv[2] ? argv[2] : "");
            p.setCreationDate(argv[3] ? argv[3] : "");
            
            postsList.push_back(p);
            return 0;
        };

        db->query(sql, callback);
        return postsList;
    };
}
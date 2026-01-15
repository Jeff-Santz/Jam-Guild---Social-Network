#include "Content/Like.h"
#include "Core/Database.h"
#include "Content/Notification.h"
#include "Core/Utils.h"


namespace Content {

    bool Like::toggle() {
        auto* db = Core::Database::getInstance();
        bool wasAdded = false; // Declaramos a variável aqui para controlar o estado

        if (Like::hasUserLiked(this->postId, this->userId)) {
            // Unlike
            std::string sql = "DELETE FROM likes WHERE post_id = " + std::to_string(this->postId) + 
                            " AND user_id = " + std::to_string(this->userId) + ";";
            
            db->execute(sql);
            wasAdded = false; 
        } else {
            // Like
            this->date = Core::Utils::getCurrentDateTime();
            std::string sql = "INSERT INTO likes (post_id, user_id, date) VALUES (" + 
                            std::to_string(this->postId) + ", " + 
                            std::to_string(this->userId) + ", '" + 
                            this->date + "');";
            
            db->execute(sql); 
            wasAdded = true;

            // Notification
            int ownerId = -1;
            auto callback = [&](int, char** argv, char**) { ownerId = std::stoi(argv[0]); return 0; };
            db->query("SELECT author_id FROM posts WHERE id=" + std::to_string(this->postId), callback);

            if (ownerId != -1) {
                Notification::create(ownerId, this->userId, Notification::LIKE, this->postId, "NOTIF_LIKE");
            }

            // Learning logic
            if (wasAdded) {
                std::string tags;
                db->query("SELECT tags FROM posts WHERE id = " + std::to_string(this->postId), [&](int, char** argv, char**) {
                    if (argv[0]) tags = argv[0];
                    return 0;
                });

                if (!tags.empty()) {
                    std::string updateInterest = "INSERT INTO user_interests (user_id, tag, weight) VALUES (" +
                        std::to_string(this->userId) + ", '" + tags + "', 1) "
                        "ON CONFLICT(user_id, tag) DO UPDATE SET weight = weight + 1;";
                    db->execute(updateInterest);
                }
            }
        }
        
        return wasAdded; // Retorna true se adicionou, false se removeu
    }

    int Like::getCount(int postId) {
        auto* db = Core::Database::getInstance();
        std::string sql = "SELECT COUNT(*) FROM likes WHERE post_id = " + std::to_string(postId) + ";";
        
        int count = 0;
        auto callback = [&](int argc, char** argv, char** colNames) -> int {
            count = std::stoi(argv[0]);
            return 0;
        };
        
        db->query(sql, callback);
        return count;
    }

    bool Like::hasUserLiked(int postId, int userId) {
        auto* db = Core::Database::getInstance();
        std::string sql = "SELECT 1 FROM likes WHERE post_id = " + std::to_string(postId) + 
                          " AND user_id = " + std::to_string(userId) + " LIMIT 1;";
        
        bool found = false;
        auto callback = [&](int argc, char** argv, char** colNames) -> int {
            found = true;
            return 0;
        };
        
        db->query(sql, callback);
        return found;
    }
}
#include "Content/Like.h"
#include "Core/Database.h"
#include "Content/Notification.h"
#include "Core/Utils.h"


namespace Content {

    bool Like::toggle() {
        auto* db = Core::Database::getInstance();
        bool wasAdded = false; 

        if (Like::hasUserLiked(this->postId, this->userId)) {
            //Remove Like
            std::string sql = "DELETE FROM likes WHERE post_id = " + std::to_string(this->postId) + 
                            " AND user_id = " + std::to_string(this->userId) + ";";
            db->execute(sql);
            //Remove Notif
            std::string sqlNotif = "DELETE FROM notifications WHERE sender_id = " + std::to_string(this->userId) + 
                                " AND post_id = " + std::to_string(this->postId) + 
                                " AND type = " + std::to_string(Notification::LIKE) + ";";
            db->execute(sqlNotif);

            wasAdded = false; 

        } else { 
            this->date = Core::Utils::getCurrentDateTime();
            //Add Like
            std::string sql = "INSERT INTO likes (post_id, user_id, date) VALUES (" + 
                            std::to_string(this->postId) + ", " + 
                            std::to_string(this->userId) + ", '" + 
                            this->date + "');";
            
            db->execute(sql); 
            wasAdded = true;

            //Add Notif
            int ownerId = -1;
            auto callback = [&](int, char** argv, char**) { ownerId = std::stoi(argv[0]); return 0; };
            db->query("SELECT author_id FROM posts WHERE id=" + std::to_string(this->postId), callback);

            if (ownerId != -1 && ownerId != this->userId) { 
                Notification::create(ownerId, this->userId, Notification::LIKE, this->postId, "NOTIF_LIKE");
            }

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
        
        return wasAdded;
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
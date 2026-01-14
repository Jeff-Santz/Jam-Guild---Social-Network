#include "Content/Post.h"
#include "Content/Notification.h"
#include "Core/Database.h"
#include "Core/Logger.h"
#include <iostream>

namespace Content {

    Post::Post() {
        this->authorId = -1;
        this->content = "";
    }

    bool Post::save() {
        auto* db = Core::Database::getInstance();
        if (this->id == -1) {
            std::string sql = "INSERT INTO posts (author_id, content, creation_date) VALUES (" +
                std::to_string(this->authorId) + ", '" + 
                this->content + "', '" + 
                this->creationDate + "');";

            if (db->execute(sql)) {
                this->id = db->getLastInsertId();
                
                // Log
                Core::Logger::log(this->authorId, "NEW_POST", "Criou um post ID: " + std::to_string(this->id));

                // Friends notification
                std::string sqlFriends = "SELECT user_id_1 FROM friendships WHERE user_id_2 = " + std::to_string(this->authorId) + " AND status = 1 "
                                         "UNION "
                                         "SELECT user_id_2 FROM friendships WHERE user_id_1 = " + std::to_string(this->authorId) + " AND status = 1;";
                
                auto callback = [&](int, char** argv, char**) {
                    int friendId = std::stoi(argv[0]);
                    Content::Notification::create(friendId, this->authorId, Content::Notification::FRIEND_POST, this->id, "NOTIF_FRIEND_POST");
                    return 0;
                };
                db->query(sqlFriends, callback);

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
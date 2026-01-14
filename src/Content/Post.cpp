#include "Content/Post.h"
#include "Core/Database.h"
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
    }
}
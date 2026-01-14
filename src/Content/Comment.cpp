#include "Content/Comment.h"
#include "Content/Notification.h"
#include "Core/Database.h"
#include "Core/Utils.h"
#include <iostream>

namespace Content {

    bool Comment::save() {
        auto* db = Core::Database::getInstance();
        this->creationDate = Core::Utils::getCurrentDateTime();

        std::string parentVal = (this->parentId == -1) ? "NULL" : std::to_string(this->parentId);

        std::string sql = "INSERT INTO comments (post_id, author_id, parent_id, content, creation_date) VALUES (" 
                          + std::to_string(this->postId) + ", " 
                          + std::to_string(this->authorId) + ", " 
                          + parentVal + ", '" 
                          + this->content + "', '" 
                          + this->creationDate + "');";

        bool success = db->execute(sql);

        if (success) {
            int ownerId = -1;
            auto callback = [&](int, char** argv, char**) { ownerId = std::stoi(argv[0]); return 0; };
            db->query("SELECT author_id FROM posts WHERE id=" + std::to_string(this->postId), callback);

            if (ownerId != -1) {
                Notification::create(ownerId, this->authorId, Notification::COMMENT, this->postId, "NOTIF_COMMENT");
            }
        }
        return success;
    }

    std::vector<Comment> Comment::getCommentsByPostId(int postId) {
        std::vector<Comment> list;
        auto* db = Core::Database::getInstance();

        // Traz tudo ordenado por data
        std::string sql = "SELECT id, author_id, parent_id, content, creation_date FROM comments WHERE post_id = " 
                          + std::to_string(postId) + " ORDER BY creation_date ASC;";

        auto callback = [&](int argc, char** argv, char** colNames) -> int {
            Comment c;
            c.setPostId(postId);
            c.setId(std::stoi(argv[0]));
            c.setAuthorId(std::stoi(argv[1]));
            
            // Trata NULL vindo do banco
            if (argv[2]) c.setParentId(std::stoi(argv[2]));
            else c.setParentId(-1);

            c.setContent(argv[3] ? argv[3] : "");
            c.setCreationDate(argv[4] ? argv[4] : "");
            list.push_back(c);
            return 0;
        };

        db->query(sql, callback);
        return list;
    }
}
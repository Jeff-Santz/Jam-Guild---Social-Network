#ifndef COMMENT_H
#define COMMENT_H

#include <string>
#include <vector>

namespace Content {

    class Comment {
    private:
        int id = -1;
        int postId = -1;
        int authorId = -1;
        int parentId = -1; 
        std::string content;
        std::string creationDate;

    public:
        bool save();
        static std::vector<Comment> getCommentsByPostId(int postId);

        // Setters
        void setId(int id) { this->id = id; }
        void setPostId(int pid) { this->postId = pid; }
        void setAuthorId(int aid) { this->authorId = aid; }
        void setParentId(int pid) { this->parentId = pid; }
        void setContent(std::string c) { this->content = c; }
        void setCreationDate(std::string d) { this->creationDate = d; }

        // Getters
        int getId() const { return id; }
        int getPostId() const { return postId; }
        int getAuthorId() const { return authorId; }
        int getParentId() const { return parentId; }
        std::string getContent() const { return content; }
        std::string getCreationDate() const { return creationDate; }


    };
}

#endif
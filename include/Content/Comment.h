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
        
        // Media
        std::string mediaUrl;
        std::string mediaType;
        
        std::string creationDate;
        int likesCount = 0;
        bool likedByMe = false;
        std::string authorName;
        std::string authorAvatar;

    public:
        int save(); // Retorna o ID do comentário criado
        static std::vector<Comment> getCommentsByPostId(int postId, int currentUserId);
        static bool toggleLike(int userId, int commentId);
        static bool deleteComment(int commentId, int requesterId, bool isAdmin);

        // Setters
        void setId(int id) { this->id = id; }
        void setPostId(int pid) { this->postId = pid; }
        void setAuthorId(int aid) { this->authorId = aid; }
        void setParentId(int pid) { this->parentId = pid; }
        void setContent(std::string c) { this->content = c; }
        void setMediaUrl(std::string u) { this->mediaUrl = u; }
        void setMediaType(std::string t) { this->mediaType = t; }
        void setCreationDate(std::string d) { this->creationDate = d; }
        
        // Auxiliary Setters
        void setLikesCount(int l) { this->likesCount = l; }
        void setLikedByMe(bool l) { this->likedByMe = l; }
        void setAuthorName(std::string n) { this->authorName = n; }
        void setAuthorAvatar(std::string a) { this->authorAvatar = a; }

        // Getters
        int getId() const { return id; }
        int getPostId() const { return postId; }
        int getAuthorId() const { return authorId; }
        int getParentId() const { return parentId; }
        std::string getContent() const { return content; }
        std::string getMediaUrl() const { return mediaUrl; }
        std::string getMediaType() const { return mediaType; }
        std::string getCreationDate() const { return creationDate; }
        int getLikesCount() const { return likesCount; }
        bool isLikedByMe() const { return likedByMe; }
        std::string getAuthorName() const { return authorName; }
        std::string getAuthorAvatar() const { return authorAvatar; }
    };
}
#endif
#ifndef POST_H
#define POST_H

#include <string>
#include <vector>
#include "Core/Entity.h"

namespace Content {

    class Post : public Core::Entity {
    private:
        int authorId;
        std::string content;
        int communityId = -1;
        std::string tags;

    public:
        Post();
        bool save() override;

        // Setters
        void setAuthorId(int id) { authorId = id; }
        void setContent(const std::string& text) { content = text; }
        void setCommunityId(int id) { communityId = id; }
        void setTags(const std::string& t) { tags = t; }

        // Getters
        static std::vector<Post> getPostsByUserId(int userId);
        int getAuthorId() const { return authorId; }
        int getCommunityId() const { return communityId; }
        std::string getTags() const { return tags; }
        std::string getContent() const { return content; }
        std::string getTableName() const override { return "posts"; }

    };

}

#endif
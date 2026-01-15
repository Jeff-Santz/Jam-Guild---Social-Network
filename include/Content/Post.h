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

    public:
        Post();
        
        int getAuthorId() const { return authorId; }
        void setAuthorId(int id) { authorId = id; }

        std::string getContent() const { return content; }
        void setContent(const std::string& text) { content = text; }

        void setCommunityId(int id) { communityId = id; }
        int getCommunityId() const { return communityId; }

        bool save() override;
        std::string getTableName() const override { return "posts"; }

        static std::vector<Post> getPostsByUserId(int userId);
    };

}

#endif
#ifndef LIKE_H
#define LIKE_H

#include <string>

namespace Content {

    class Like {
    private:
        int postId;
        int userId;
        std::string date;

    public:
        bool toggle(); 
        static bool hasUserLiked(int postId, int userId); 

        // Setters
        void setPostId(int p) { postId = p; }
        void setUserId(int u) { userId = u; }

        // Getters
        static int getCount(int postId);
    };
}

#endif
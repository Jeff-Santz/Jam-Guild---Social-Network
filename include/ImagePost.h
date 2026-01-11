#ifndef IMAGEPOST_H
#define IMAGEPOST_H

#include "Post.h"
#include <string>

class ImagePost : public Post {
private:
    std::string imagePath;

public:
    ImagePost(std::string text, Profile* owner, std::string imagePath);
    ImagePost(std::string text, std::time_t date, Profile* owner, std::string imagePath);

    void print();
    std::string getMediaPath() { return imagePath; }
    std::string getType() { return "IMAGE"; }
};

#endif
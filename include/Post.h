#ifndef POST_H
#define POST_H

#include <string>
#include <ctime>
#include <vector>
#include "Comment.h"

class Profile;  // Forward declaration

class Post {
protected:
    std::string text;
    std::time_t date;
    Profile* owner;
    std::vector<Profile*> likes;      
    std::vector<Comment*> comments;  

public:
    Post(std::string text, Profile* owner); 
    Post(std::string text, std::time_t date, Profile* owner);
    virtual ~Post();                                     
    virtual void print();
    void addLike(Profile* p);
    int getLikesCount() const;
    void addComment(std::string text, Profile* author);
    const std::vector<Comment*>& getComments() const;

    // Setters
    void setOwner(Profile* newOwner);
    
    // Getters
    std::string getText();
    std::time_t getDate();
    std::string getFormattedDate();
    Profile* getOwner();
    virtual std::string getType() { return "TEXT"; }
    virtual std::string getMediaPath() { return ""; }

};

#endif

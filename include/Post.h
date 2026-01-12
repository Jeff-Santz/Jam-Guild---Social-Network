#ifndef POST_H
#define POST_H

#include <string>
#include <ctime>

class Profile;  // Forward declaration

class Post {
private:
    std::string text;
    std::time_t date;
    Profile* owner;

public:
    Post(std::string text, Profile* owner); 
    Post(std::string text, std::time_t date, Profile* owner);
    virtual ~Post();                                     
    virtual void print();

    // Setters
    void setOwner(Profile* newOwner);
    
    // Getters
    std::string getText();
    std::time_t getDate();
    std::string getFormattedDate();
    Profile* getOwner();
    virtual std::string getType() { return "TEXT"; }
    virtual std::string getMediaPath() { return ""; }
    // std::string getContent();

};

#endif

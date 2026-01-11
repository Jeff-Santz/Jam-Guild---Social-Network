#ifndef POST_H
#define POST_H

#include <string>

class Profile;  // Forward declaration

class Post {
private:
    std::string text;
    int date;
    Profile* owner;

public:
    Post(std::string text, int date, Profile* owner); 
    ~Post();                                     
    void print();
    
    // Getters
    std::string getText();
    int getDate();
    Profile* getOwner();
    // std::string getContent();

};

#endif

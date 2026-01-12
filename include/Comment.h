#ifndef COMMENT_H
#define COMMENT_H

#include <string>
#include <ctime>

class Profile; // Forward declaration

class Comment {
private:
    std::string text;
    std::time_t date;
    Profile* author;

public:
    Comment(std::string text, Profile* author);
    
    // Getters
    std::string getText() const;
    Profile* getAuthor() const;
    std::string getFormattedDate() const;
};

#endif
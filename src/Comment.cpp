#include "Comment.h"
#include "Profile.h"
#include <iomanip>
#include <sstream>

Comment::Comment(std::string text, Profile* author) {
    this->text = text;
    this->author = author;
    this->date = std::time(nullptr); 
}

std::string Comment::getText() const {
    return text;
}

Profile* Comment::getAuthor() const {
    return author;
}

std::string Comment::getFormattedDate() const {
    char buffer[80];
    struct tm* timeinfo = std::localtime(&date);
    std::strftime(buffer, 80, "%d/%m/%Y %H:%M", timeinfo);
    return std::string(buffer);
}
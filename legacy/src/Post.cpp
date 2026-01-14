#include "Post.h"
#include "Profile.h"  
#include "User.h"
#include <string>
#include <stdexcept>
using namespace std;

//Construtores e Destrutores
Post::Post(string text, Profile* owner) {
    this->text = text;
    this->owner = owner;
    this->date = std::time(nullptr); // Pega a hora atual do sistema
}

Post::Post(string text, time_t date, Profile* owner) {
    this->text = text;
    this->owner = owner;
    this->date = date;
}

Post::~Post() {
    for (Comment* c : comments) {
        delete c; 
    }
    comments.clear();
    cout << "Post destructor: " << this->text << endl;
}

void Post::print() {
    cout << "Text: " << getText() << " - Date: " << getFormattedDate() << " - Owner: " << owner->getName() << endl;
}

void Post::addLike(Profile* p) {
    // Verifica se o perfil já está na lista de curtidas
    for (Profile* existing : likes) {
        if (existing == p) {
            throw std::logic_error("You already liked this post!");
        }
    }
    likes.push_back(p);

    User* owner = dynamic_cast<User*>(this->owner);
    if (owner && p != this->owner) {
        std::string msg = owner->getName() + " Liked your post!";
        owner->addNotification(new Notification(msg));
    }
}

void Post::addComment(std::string text, Profile* author) {
    if (text.empty()) return;
    comments.push_back(new Comment(text, author));

    User* owner = dynamic_cast<User*>(this->owner);
    if (owner && author != owner) {
        std::string msg = author->getName() + " commented on your post!";
        owner->addNotification(new Notification(msg));
    }
}

void Post::setOwner(Profile* newOwner) {
    this->owner = newOwner;
}

string Post::getFormattedDate() {
    char buffer[80];
    struct tm* timeinfo = localtime(&date);
    strftime(buffer, 80, "%d/%m/%Y %H:%M", timeinfo);
    return string(buffer);
}

const std::vector<Profile*>& Post::getLikes() const {
    return likes;
}

const std::vector<Comment*>& Post::getComments() const {
    return comments;
}

string Post::getText() { return this->text; }
int Post::getLikesCount() const { return likes.size(); }
time_t Post::getDate() {return this->date;}
Profile* Post::getOwner() {return this->owner; }





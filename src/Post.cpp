#include "Post.h"
#include "Profile.h"  
#include <string>
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
    cout << "Post destructor: " << this->text << endl;
}

void Post::print() {
    cout << "Text: " << getText() << " - Date: " << getFormattedDate() << " - Owner: " << owner->getName() << endl;
}

string Post::getFormattedDate() {
    char buffer[80];
    struct tm* timeinfo = localtime(&date);
    strftime(buffer, 80, "%d/%m/%Y %H:%M", timeinfo);
    return string(buffer);
}

string Post::getText() { return this->text; }

time_t Post::getDate() {return this->date;}

Profile* Post::getOwner() {return this->owner; }





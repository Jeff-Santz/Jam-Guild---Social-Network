#include "Post.h"
#include "Profile.h"  
#include <string>
using namespace std;

//Construtores e Destrutores
Post::Post(string text, int date, Profile* owner) {
    this->text=text;
    this->date=date;
    this->owner=owner;
 };

Post::~Post() {
    cout << "Post destructor: " << this->text << endl;
}

void Post::print() {
    cout << "Text: " << getText() << " - Date: " << getDate() << " - Owner: " << owner->getName() << endl;
}

string Post::getText() { return this->text; }

int Post::getDate() {return this->date;}

Profile* Post::getOwner() {return this->owner; }





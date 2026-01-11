#include "Page.h"
#include "Profile.h"
#include "VerifiedUser.h"
#include <iostream>
#include <string>
using namespace std;


Page::Page(string name, VerifiedUser* owner, string password) : Profile(name, password, "page_default.png", "Pagina Oficial", "Organizacao/Empresa", "Data de Fundacao") {
    this->owner = owner;
    if (owner) Profile::addContact(owner);
}

Page::Page(string name, VerifiedUser* owner, string password, string icon, string bio, string subtitle, string startDate) : Profile(name, password, icon, bio, subtitle, startDate) {
    this->owner = owner;
    if (owner) Profile::addContact(owner);
}

Page::~Page() {
}

void Page::addPost(Post* p) {
	posts.push_back(p);
}

void Page::setOwner(VerifiedUser* newOwner) {
    this->owner = newOwner;

    if (newOwner != nullptr) {
        Profile::addContact(newOwner);
    }
}

void Page::print() {
    cout << "Name: " << getName() << " - id: " << getId();
	
    if (owner != nullptr) {
        cout << " - Owner: " << owner->getName();
    } else {
        cout << " - Owner: [Unknown]";
    }
}

VerifiedUser* Page::getOwner() {
    return this->owner;
}





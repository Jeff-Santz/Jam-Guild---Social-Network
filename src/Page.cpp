#include "Page.h"
#include "Profile.h"
#include "VerifiedUser.h"
#include <iostream>
#include <string>
using namespace std;


Page::Page(string name, VerifiedUser* owner, int max) : Profile(name, max) {
    this->owner = owner;

    Profile::addContact(owner);
};

Page::~Page() {
}

void Page::addPost(Post* p) {
	posts.push_back(p);
}


VerifiedUser* Page::getOwner() {
    return this->owner;
}

void Page::print() {
    cout << "Name: " << getName() << " - id: " << getId()
	<< " - Owner: " << owner->getName() << endl;
}




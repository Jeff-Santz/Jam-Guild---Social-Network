#include "User.h"
#include <iostream>
#include <string>
using namespace std;

User::User(std::string name, std::string password) 
    : Profile(name, password, "user_default.png", "Hi! I am new here.", "Undefined", "01/01/2024") {
    this->isVerifiedStatus = false;
    this->Email = "";
}

User::User(std::string name, std::string password, std::string icon, std::string bio, std::string subtitle, std::string startDate, bool isVerified, std::string email)
    : Profile(name, password, icon, bio, subtitle, startDate) {
    this->isVerifiedStatus = isVerified;
    this->Email = email;
}

User::~User() {};

void User::addPost(Post* p) {
	posts.push_back(p);
}

bool User::isVerified() {
	return this->isVerifiedStatus;
}

void User::verify(std::string email) {
	this->isVerifiedStatus = true;
	this->Email = email;
}	

string User::getEmail() {
	return this->Email;
}
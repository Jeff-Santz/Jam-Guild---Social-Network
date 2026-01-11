#include "User.h"
#include <iostream>
#include <string>
using namespace std;

User::User(string name, string password)  : Profile(name, password, "user_default.png", "Ola! Sou novo aqui.", "Indefinido", "01/01/2000") {};

User::User(string name, string password, string icon, string bio, string subtitle, string startDate): Profile(name, password, icon, bio, subtitle, startDate) {};

User::~User() {};

void User::addPost(Post* p) {
	posts.push_back(p);
}


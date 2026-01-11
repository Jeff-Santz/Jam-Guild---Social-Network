#include "User.h"
#include <iostream>
#include <string>
using namespace std;

User::User(string name, string password) : Profile(name, password) {};

User::~User() {};

void User::addPost(Post* p) {
	posts.push_back(p);
}


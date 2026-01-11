#include "User.h"
#include <iostream>
#include <string>
using namespace std;

User::User(string name, int max) : Profile(name, max) {};

User::~User() {};

void User::addPost(Post* p) {
	posts.push_back(p);
}


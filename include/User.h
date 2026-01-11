#ifndef USER_H
#define USER_H

#include "Profile.h"
#include <iostream>
#include <string>

class User : public Profile {
public:
    User(std::string name, int max);
    virtual ~User();
    virtual void addPost(Post* p);
};

#endif
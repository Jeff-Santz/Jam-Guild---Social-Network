#ifndef USER_H
#define USER_H

#include "Profile.h"
#include <iostream>
#include <string>

class User : public Profile {
public:
    User(std::string name, std::string password);
    virtual ~User();
    virtual void addPost(Post* p);
    virtual std::string getRole() const override { return "Standard User"; }
};

#endif
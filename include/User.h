#ifndef USER_H
#define USER_H

#include "Profile.h"
#include <iostream>
#include <string>

class User : public Profile {
public:
    // Conta Nova
    User(std::string name, std::string password);

    //Carregar do BD
    User(std::string name, std::string password, std::string icon, std::string bio, std::string subtitle, std::string startDate);

    virtual ~User();
    virtual void addPost(Post* p);
    virtual std::string getRole() const override { return "Standard User"; }
};

#endif
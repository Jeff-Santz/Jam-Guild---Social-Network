#ifndef VERIFIEDUSER_H
#define VERIFIEDUSER_H

#include "User.h"
#include <iostream>
#include <string>

class VerifiedUser : public User {
private:
    std::string email;
    
public:
    VerifiedUser(std::string name, std::string email);
    VerifiedUser(std::string name);

    std::string getEmail();
    virtual void print();
};

#endif
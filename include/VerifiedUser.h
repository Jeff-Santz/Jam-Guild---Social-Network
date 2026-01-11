#ifndef VERIFIEDUSER_H
#define VERIFIEDUSER_H

#include "User.h"
#include <iostream>
#include <string>

class VerifiedUser : public User {
private:
    std::string email;
    
public:
    VerifiedUser(std::string name, std::string email, std::string password);
    VerifiedUser(std::string name, std::string password);

    std::string getEmail();
    virtual std::string getRole() const override { return "Verified User"; }
    virtual void print();
};

#endif
#ifndef VERIFIEDUSER_H
#define VERIFIEDUSER_H

#include "User.h"
#include <iostream>
#include <string>

class VerifiedUser : public User {
private:
    std::string email;
    
public:
    // Conta Nova
    VerifiedUser(std::string name, std::string email, std::string password);
    VerifiedUser(std::string name, std::string password);

    //Carregar do BD
    VerifiedUser(std::string name, std::string email, std::string password, std::string icon, std::string bio, std::string subtitle, std::string startDate);

    std::string getEmail();
    virtual std::string getRole() const override { return "Verified User"; }
    virtual void print();
};

#endif
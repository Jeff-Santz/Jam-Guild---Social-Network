#ifndef USER_H
#define USER_H

#include "Profile.h"
#include "Notification.h"
#include <iostream>
#include <string>
#include <memory>
#include <vector>

class User : public Profile {
private:
    bool isVerifiedStatus;   
    std::string Email; 
    std::vector<std::unique_ptr<Notification>> notifications;

public:
    // Conta Nova
    User(std::string name, std::string password);

    //Carregar do BD
    User(std::string name, std::string password, std::string icon, std::string bio, std::string subtitle, std::string startDate, bool isVerified, std::string email);
    
    virtual ~User();
    virtual void addPost(Post* p);
    void addNotification(Notification* n);
    const std::vector<std::unique_ptr<Notification>>& getNotifications() const;
    void clearNotifications();
    void verify(std::string email);
    bool isVerified();

    virtual std::string getRole() const override { 
        return isVerifiedStatus ? "Verified User" : "Standard User"; 
    }
    std::string getEmail();


};

#endif
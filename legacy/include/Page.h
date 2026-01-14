#ifndef PAGE_H
#define PAGE_H

#include "Profile.h"
#include "User.h"
#include <string>

class Page : public Profile {
private:
    User* owner;

public:
    Page(std::string name, User* owner);
    Page(int id, std::string name, User* owner, std::string icon, std::string bio, std::string subtitle, std::string startDate);

    virtual ~Page();

    void setOwner(User* newOwner);
    User* getOwner();
    
    virtual std::string getRole() const override { return "PAGE"; }
    virtual void print() override;
    
    virtual void addPost(Post* p) override; 
};

#endif
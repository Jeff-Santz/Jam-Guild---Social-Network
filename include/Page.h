#ifndef PAGE_H
#define PAGE_H

#include "Profile.h"
#include "Post.h"
#include "User.h"
#include <iostream>
#include <string>

class Page : public Profile {
private:
    User* owner;

public:
    // Pagina Nova
    Page(std::string name, User* owner, std::string password);

    //Carregar do BD
    Page(std::string name, User* owner, std::string password, std::string icon, std::string bio, std::string subtitle, std::string startDate);
    virtual ~Page();

    void addPost(Post* p);
    void setOwner(User* newOwner);
    User* getOwner();
    virtual std::string getRole() const override { return "Page"; }
    virtual void print();
};

#endif
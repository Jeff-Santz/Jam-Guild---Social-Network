#ifndef PAGE_H
#define PAGE_H

#include "Profile.h"
#include "VerifiedUser.h"
#include "Post.h"
#include <iostream>
#include <string>

class Page : public Profile {
private:
    VerifiedUser* owner;

public:
    Page(std::string name, VerifiedUser* owner);
    virtual ~Page();

    void addPost(Post* p);
    void setOwner(VerifiedUser* newOwner);
    VerifiedUser* getOwner();
    virtual std::string getRole() const override { return "Page"; }
    virtual void print();
};

#endif
#ifndef PROFILE_H
#define PROFILE_H

#include "Post.h"
#include <list>
#include <iostream>
#include <string>
#include <vector>

class Profile {
protected:
    std::string name;
    std::vector<Profile*> contacts;
    std::list<Post*> posts;     

    static int Lastid;    
    int id;    

public:
    Profile(std::string name); 
    virtual ~Profile();               

    void addContact(Profile* contact);     
    virtual void addPost(Post* p) = 0;    //Abstract Class
    virtual void print();

    //Getters
    std::string getName();
    int getContactsAmount();
    std::vector<Profile*>* getContacts();
    std::list<Post*>* getPosts();
    std::list<Post*>* getContactsPosts();


    int getId();
    static int getLastId();

};

#endif

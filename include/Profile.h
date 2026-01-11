#ifndef PROFILE_H
#define PROFILE_H

#include "Post.h"
#include <list>
#include <iostream>
#include <string>

class Profile {
protected:
    std::string name;
    Profile** contacts;      
    int contactsAmount;
    std::list<Post*> posts;    
    int max;     

    static int Lastid;    
    int id;    

public:
    Profile(std::string name, int max); 
    virtual ~Profile();               

    void addContact(Profile* contact);     
    virtual void addPost(Post* p) = 0;    //Abstract Class
    virtual void print();

    //Getters
    std::string getName();
    int getContactsAmount();
    Profile** getContacts();
    std::list<Post*>* getPosts();
    std::list<Post*>* getContactsPosts();


    int getId();
    static int getLastId();

};

#endif

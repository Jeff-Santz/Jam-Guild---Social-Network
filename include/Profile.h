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
    std::string password;
    std::vector<Profile*> contacts;
    std::list<Post*> posts;     

    static int Lastid;    
    int id;    

public:
    Profile(std::string name, std::string password);
    virtual ~Profile();               

    bool checkPassword(std::string passAttempt);
    void addContact(Profile* contact);     
    virtual void addPost(Post* p) = 0;    //Abstract Class
    virtual void print();

    void setId(int newId);

    //Getters
    std::string getName();
    std::string getPassword(); //Colocar criptografia depois
    int getContactsAmount();
    std::vector<Profile*>* getContacts();
    std::list<Post*>* getPosts();
    std::list<Post*>* getContactsPosts();
    virtual std::string getRole() const { return "Profile"; }


    int getId();
    static int getLastId();

};

#endif

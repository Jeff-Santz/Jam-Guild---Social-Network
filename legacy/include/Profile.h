#ifndef PROFILE_H
#define PROFILE_H

#include "Post.h"
#include <list>
#include <iostream>
#include <string>
#include <vector>

class User;

class Profile {
protected:
    std::string name;
    std::string password;
    std::vector<Profile*> contacts;
    std::list<Post*> posts; 
    std::vector<Profile*> contactRequests;  
    
    // Referentes ao Perfil
    std::string iconPath;
    std::string bio;       
    std::string subtitle;  // Genero para Pessoa e Categoria Para Pagina
    std::string startDate; // Data de Nascimento para Pessoa e Data de Criacao para Pagina
    // -----------------------

    static int Lastid;    
    int id;    

public:
    Profile(std::string name, std::string password, 
            std::string icon = "default.png", 
            std::string bio = "Ola! Estou usando a Rede Social.", 
            std::string subtitle = "Nao informado", 
            std::string startDate = "01/01/2000");

    virtual ~Profile();               

    bool checkPassword(std::string passAttempt);
    void addContactRequest(Profile* requester);
    void removeRequest(Profile* requester);
    void addContact(Profile* contact); 
    void removeContact(Profile* contact);    
    virtual void addPost(Post* p) = 0;    //Abstract Class
    virtual void print();

    // Setters
    void setIconPath(std::string path);
    void setId(int newId);
    void setBio(std::string text);
    void setSubtitle(std::string text); 
    void setStartDate(std::string date); 

    //Getters
    std::string getName();
    std::string getPassword(); //Colocar criptografia depois
    int getContactsAmount();
    std::string getIconPath();
    std::string getBio();
    std::string getSubtitle();
    std::string getStartDate();
    std::vector<Profile*>* getContacts();
    std::list<Post*>* getPosts();
    std::list<Post*>* getContactsPosts();
    std::vector<Profile*>& getContactRequests();
    virtual std::string getRole() const { return "Profile"; }


    int getId();
    static int getLastId();

};

#endif

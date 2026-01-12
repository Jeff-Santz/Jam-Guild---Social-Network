#include "Profile.h"
#include "Utils.h"
#include <iostream>
#include <string>
#include <stdexcept>
#include <algorithm>
using namespace std;

int Profile::Lastid = 0;

// Construtores e Destrutores
Profile::Profile(string name, string password, string icon, string bio, string subtitle, string startDate) {
    this->name = name;
    this->password = Utils::hashPassword(password);
    this->iconPath = icon;
    this->bio = bio;
    this->subtitle = subtitle;
    this->startDate = startDate;
    
    Profile::Lastid++;
    this->id = Lastid;
}

Profile::~Profile() {

	 cout << "Profile destructor: " << name << " - Number of posts made:: "
	<< posts.size() << endl;

	for (Post* p : posts) {
		delete p;
	}
    posts.clear();

	cout << "Profile deleted" << endl; 
}

bool Profile::checkPassword(string hashedPassAttempt) {
    return this->password == hashedPassAttempt;
}

void Profile::print() {
    // Perfil
    cout << "\n========================================" << endl;
    cout << "PROFILE: " << name << " (ID: " << getId() << ")" << endl;
    cout << "Profile Picture: [" << iconPath << "]" << endl;
    cout << "Gender: " << subtitle << " | Birthdate: " << startDate << endl;
    cout << "Bio: " << bio << endl;
    cout << "========================================" << endl;

    // MEUS POSTS 
    cout << ">> My Posts (" << posts.size() << "):" << endl;
    if (posts.empty()) {
        cout << "   (No posts yet)" << endl;
    } else {
        for (Post* p : posts) {
            cout << "   [" << p->getFormattedDate() << "] " << p->getText() << endl;
        }
    }

    // Amigos
    cout << "\n>> Network Feed:" << endl;
    if (contacts.size() == 0) {
        cout << "   No Contacts." << endl;
    } else {
        for(int i = 0; i < contacts.size(); i++) {
            cout << "   --- From " << contacts[i]->getName() << " ---" << endl;
            
            for (Post* p : contacts[i]->posts) {
                cout << "   [" << p->getFormattedDate() << "] "
                     << contacts[i]->getName() << " posted: "
                     << p->getText() 
                     << endl;
            }
        }
    }
    cout << "========================================\n" << endl;
}

void Profile::addContact(Profile* contact) {

	//Erros de Perfil ja adicionado ou Perfil adicionando ele mesmo
	if (this == contact) {
		throw invalid_argument ("Profile adding itself");
	}

	
	auto it = std::find(contacts.begin(), contacts.end(), contact);
	if (it != contacts.end()) {
		throw invalid_argument("Already added profile");
	}

	this->contacts.push_back(contact);
	contact->contacts.push_back(this);
}

void Profile::setId(int newId) {
    this->id = newId;

    if (newId > Lastid) {
        Lastid = newId;
    }
}
//Setters
void Profile::setIconPath(string path) { this->iconPath = path; }
void Profile::setBio(string text) { this->bio = text; }
void Profile::setSubtitle(string text) { this->subtitle = text; }
void Profile::setStartDate(string date) { this->startDate = date; }

//Getters
string Profile::getName() { return this->name; }
string Profile::getPassword() { return this->password; }
int Profile::getContactsAmount() { return this->contacts.size(); }
std::vector<Profile*>* Profile::getContacts() { return &this->contacts; }
list<Post*>* Profile::getPosts() { return &this->posts;}
int Profile:: getId() { return this->id; };
int Profile::getLastId() { return Profile::Lastid;};
string Profile::getSubtitle() { return this->subtitle; }
string Profile::getStartDate() { return this->startDate; }
string Profile::getIconPath() { return this->iconPath; } 
string Profile::getBio() { return this->bio; }           

list<Post*>* Profile::getContactsPosts() {

    list<Post*>* SuperFinalList = new list<Post*>();

    for (int i = 0; i < contacts.size(); i++) {
		// Primeiro eu copio as postagens do contato pra uma lista genérica
        list<Post*>* contactPosts = contacts[i]->getPosts();

        // E depois eu junto postagens, uma a uma na SuperListaFinal
        SuperFinalList->insert(SuperFinalList->end(), contactPosts->begin(), contactPosts->end());
    }

    return SuperFinalList;
}


#include "Profile.h"
#include <iostream>
#include <string>
#include <stdexcept>
#include <algorithm>
using namespace std;

int Profile::Lastid = 0;

// Construtores e Destrutores
Profile::Profile(string name) {
    this->name = name;
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

void Profile::print() {
	cout << endl << "Name: " << name << " - id: " << getId() << endl;
	cout << "Number of posts made: " << posts.size() << endl;
	for (Post* p : posts)
		cout << "Posts from date " << p->getDate()
		<< " - Text: " << p->getText() << endl;
	if (contacts.size() == 0) {
		cout << "No Contacts " << endl;
	} else {
		for(int i = 0; i < contacts.size(); i++) {
			for (Post* p : contacts[i]->posts)
				cout << "Posts from date "
				<< p->getDate()
				<< " from contact " << contacts[i]->getName()
				<< " - Text: " << p->getText()
				<< endl;
		}
	}		
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

 // Getters

string Profile::getName() { return this->name; }

int Profile::getContactsAmount() { return this->contacts.size(); }

std::vector<Profile*>* Profile::getContacts() { return &this->contacts; }

list<Post*>* Profile::getPosts() { return &this->posts;}

int Profile:: getId() { return this->id; };

int Profile::getLastId() { return Profile::Lastid;};

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

#include "Page.h"
#include "Profile.h"
#include "Utils.h"
#include <iostream>

using namespace std;

// Construtor de Página NOVA
// Note: Passamos "" como senha para o Profile. Página não tem senha própria.
Page::Page(string name, User* owner) 
    : Profile(name, "", "page_default.png", "Pagina Oficial", "Organizacao", Utils::getCurrentDate()) 
{
    this->owner = owner;
    if (owner) {
        Profile::addContact(owner); // O dono segue a página automaticamente
    }
}

// Construtor de Carga (BD)
// Note: Recebemos o ID e forçamos ele (precisamos garantir que Profile tenha setId acessível ou usar o Lastid)
Page::Page(int id, string name, User* owner, string icon, string bio, string subtitle, string startDate) 
    : Profile(name, "", icon, bio, subtitle, startDate) 
{
    this->id = id; // Acesso direto pois 'id' é protected em Profile.h
    this->owner = owner;
    // Não adicionamos contato aqui pois isso já deve estar salvo na tabela de conexões
}

Page::~Page() {
    // Destrutor padrão
}

void Page::addPost(Post* p) {
    posts.push_back(p);
}

void Page::setOwner(User* newOwner) {
    this->owner = newOwner;
    if (newOwner != nullptr) {
        Profile::addContact(newOwner);
    }
}

User* Page::getOwner() {
    return this->owner;
}

void Page::print() {
    cout << "[PAGE] " << getName() << " (ID: " << getId() << ")";
    if (owner) cout << " | Admin: " << owner->getName();
    cout << endl;
}
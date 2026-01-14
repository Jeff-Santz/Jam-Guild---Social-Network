#include "VerifiedUser.h"
#include <iostream>
#include <string>
using namespace std;

VerifiedUser::VerifiedUser(string name, string email, string password) : User(name, password, "user_default.png", "Ola! Sou novo aqui.", "Indefinido", "01/01/2000") {
    this->email = email;
}

VerifiedUser::VerifiedUser(string name, string email, string password, string icon, string bio, string subtitle, string startDate): User(name, password, icon, bio, subtitle, startDate) {
    this->email = email;
}

VerifiedUser::VerifiedUser(string name, string password) : User(name, password, "verified_default.png", "Conta Oficial Verificada", "Indefinido", "01/01/2000") {
    this->email = "void@generic.com";
}

string VerifiedUser:: getEmail() {
    return this->email;
};

void VerifiedUser::print() {
    cout << "Email: " << email << endl;
    Profile::print();
}
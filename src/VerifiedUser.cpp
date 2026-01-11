#include "VerifiedUser.h"
#include <iostream>
#include <string>
using namespace std;

VerifiedUser::VerifiedUser(string name, string email, string password) : User(name, password) {
    this->email = email;
};

VerifiedUser::VerifiedUser(string name, string password) : User(name, password) {
    this->email = "void@generic.com";
};

string VerifiedUser:: getEmail() {
    return this->email;
};

void VerifiedUser::print() {
    cout << "Email: " << email << endl;
    Profile::print();
}
#include "PessoaVerificada.h"
#include <iostream>
#include <string>
using namespace std;

PessoaVerificada::PessoaVerificada(string nome, string email, int maximo) : Pessoa(nome, maximo) {
    this->email = email;
};

PessoaVerificada::PessoaVerificada(string nome, int maximo) : Pessoa(nome, maximo) {
    this->email = "vazio@usp.br";
};

string PessoaVerificada:: getEmail() {
    return this->email;
};

void PessoaVerificada::imprimir() {
    cout << "Email: " << email << endl;
    Perfil::imprimir();
}
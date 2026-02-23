#ifndef PESSOAVERIFICADA_H
#define PESSOAVERIFICADA_H

#include "Pessoa.h"
#include <iostream>
#include <string>
using namespace std;

class PessoaVerificada : public Pessoa {
public:
    string email;
    PessoaVerificada(string nome, string email, int maximo);
    PessoaVerificada(string nome, int maximo);

    string getEmail();
    virtual void imprimir();
};

#endif
#ifndef PESSOA_H
#define PESSOA_H

#include "Perfil.h"
#include <iostream>
#include <string>
using namespace std;

class Pessoa : public Perfil {
public:
    Pessoa(string nome, int maximo);
    virtual ~Pessoa();
    virtual void adicionarPostagem(Postagem* p);
};

#endif
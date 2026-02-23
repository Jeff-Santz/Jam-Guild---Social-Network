#ifndef PAGINA_H
#define PAGINA_H

#include "Perfil.h"
#include "PessoaVerificada.h"
#include "Postagem.h"
#include <iostream>
#include <string>
using namespace std;

class Pagina : public Perfil {
private:
    PessoaVerificada* proprietario;

public:
    Pagina(string nome, PessoaVerificada* proprietario, int maximo);
    virtual ~Pagina();

    void adicionarPostagem(Postagem* p);
    PessoaVerificada* getProprietario();
    virtual void imprimir();
};

#endif
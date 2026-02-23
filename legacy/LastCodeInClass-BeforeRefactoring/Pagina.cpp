#include "Pagina.h"
#include "Perfil.h"
#include "PessoaVerificada.h"
#include <iostream>
#include <string>
using namespace std;


Pagina::Pagina(string nome, PessoaVerificada* proprietario, int maximo) : Perfil(nome, maximo) {
    this->proprietario = proprietario;

    Perfil::adicionarContato(proprietario);
};

Pagina::~Pagina() {
}

void Pagina::adicionarPostagem(Postagem* p) {
	postagens.push_back(p);
}


PessoaVerificada* Pagina::getProprietario() {
    return this->proprietario;
}

void Pagina::imprimir() {
    cout << "Nome: " << getNome() << " - id: " << getId()
	<< " - Proprietario: " << proprietario->getNome() << endl;
}




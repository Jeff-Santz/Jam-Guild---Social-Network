#include "Pessoa.h"
#include <iostream>
#include <string>
using namespace std;

Pessoa::Pessoa(string nome, int maximo) : Perfil(nome, maximo) {};

Pessoa::~Pessoa() {};

void Pessoa::adicionarPostagem(Postagem* p) {
	postagens.push_back(p);
}


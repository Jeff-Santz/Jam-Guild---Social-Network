#include "Perfil.h"
#include <iostream>
#include <string>
#include <stdexcept>
using namespace std;

int Perfil::Ultimoid=0;

void Perfil::imprimir() {
	cout << endl << "Nome: " << nome << " - id: " << getId() << endl;
	cout << "Numero de postagens feitas: " << postagens.size() << endl;
	for (Postagem* p : postagens)
		cout << "Postagens na data " << p->getData()
		<< " - Texto: " << p->getTexto() << endl;
	if (quantidadeDeContatos == 0)
		cout << "Sem contatos " << endl;
	else {
		for(int i = 0; i < quantidadeDeContatos; i++) {
			for (Postagem* p : contatos[i]->postagens)
				cout << "Postagens na data "
				<< p->getData()
				<< " do contato " << contatos[i]->getNome()
				<< " - Texto: " << p->getTexto()
				<< endl;
		}
	}		
} 

void Perfil::adicionarContato(Perfil* contato) {

	// Erro de maximo de contatos
	if (this->quantidadeDeContatos >= maximo || contato->quantidadeDeContatos >= contato->maximo) {
		throw new overflow_error ("Maximo de contatos");
	}	 

	//Erros de Perfil ja adicionado ou Perfil adicionando ele mesmo
	if (this == contato) {
		throw new invalid_argument ("Perfil adicionando ele mesmo");
	}

	for(int i=0; i < this->quantidadeDeContatos; i++) {
		if (this->contatos[i]==contato){
			throw new invalid_argument ("Perfil ja adicionado");
		}
	}


	this->contatos[this->quantidadeDeContatos] = contato;
	this->quantidadeDeContatos ++;

	contato->contatos[contato->quantidadeDeContatos] = this;
	contato->quantidadeDeContatos ++;

}

 // Getters

string Perfil::getNome() {
	return this->nome;
}

int Perfil::getQuantidadeDeContatos() {
	return this->quantidadeDeContatos;
}

Perfil** Perfil::getContatos() {
	return this->contatos;
}

list<Postagem*>* Perfil::getPostagens() {
    return &this->postagens;
}

list<Postagem*>* Perfil::getPostagensDosContatos() {

    list<Postagem*>* SuperListaFinal = new list<Postagem*>();

    for (int i = 0; i < quantidadeDeContatos; i++) {
		// Primeiro eu copio as postagens do contato pra uma lista genérica
        list<Postagem*>* postagensContato = contatos[i]->getPostagens();

        // E depois eu junto postagens, uma a uma na SuperListaFinal
        SuperListaFinal->insert(SuperListaFinal->end(), postagensContato->begin(), postagensContato->end());
    }

    return SuperListaFinal;
}


int Perfil:: getId() {
	return this->id;
};
int Perfil::getUltimoId() {
	return Perfil::Ultimoid;
};

// Construtores e Destrutores
Perfil::Perfil(string nome, int maximo) {
    this->nome = nome;
    this->maximo = maximo;
	Perfil::Ultimoid++;
	this->id = Ultimoid;

    contatos = new Perfil*[maximo];
}

Perfil::~Perfil() {

	 cout << "Destrutor de perfil: " << nome << " - Quantidade de postagens feitas: "
	<< postagens.size() << endl;

	for (Postagem* p : postagens) {
		delete p;
	}
    postagens.clear();

    delete[] contatos;

	cout << "Perfil deletado" << endl; 
}

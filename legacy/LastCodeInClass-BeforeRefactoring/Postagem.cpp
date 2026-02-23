#include "Postagem.h"
#include "Perfil.h"  
#include <string>
using namespace std;

// Getter do texto
string Postagem::getTexto() {
    return this->texto;
}

// Getter da data
int Postagem::getData() {
    return this->data;
}

// Getter do autor
Perfil* Postagem::getAutor() {
    return this->autor;
}

Postagem::Postagem(string texto, int data, Perfil* autor) {
    this->texto=texto;
    this->data=data;
    this->autor=autor;
 };

Postagem::~Postagem() {
    cout << "Destrutor de postagem: " << this->texto << endl;
 }

void Postagem::imprimir() {
    cout << "Texto: " << getTexto() << " - Data: " << getData() << " - Autor: " << autor->getNome() << endl;
}


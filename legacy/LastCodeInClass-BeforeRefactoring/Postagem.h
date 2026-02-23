#ifndef POSTAGEM_H
#define POSTAGEM_H

#include <string>
using namespace std;

// Declaração antecipada (protótipo)
class Perfil;

class Postagem {
private:
    string texto;
    int data;
    Perfil* autor;

public:
    Postagem(string texto, int data, Perfil* autor); // Construtor
    ~Postagem();                                     // Destrutor
    void imprimir();
    
    // Getters
    string getTexto();
    int getData();
    Perfil* getAutor();

};

#endif

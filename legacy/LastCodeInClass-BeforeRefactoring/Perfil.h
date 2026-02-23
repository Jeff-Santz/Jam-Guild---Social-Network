#ifndef PERFIL_H
#define PERFIL_H

#include "Postagem.h"
#include <list>
#include <iostream>
#include <string>
using namespace std;

class Perfil {
protected:
    string nome;
    Perfil** contatos;      
    int quantidadeDeContatos = 0;
    list<Postagem*> postagens;    
    int maximo;     

    static int Ultimoid;    
    int id;    

public:
    Perfil(string nome, int maximo); // Construtor
    virtual ~Perfil();               // Destrutor

    void adicionarContato(Perfil* contato);     
    virtual void adicionarPostagem(Postagem* p) = 0;    //Método Abstrato
    virtual void imprimir();

    //Getters
    string getNome();
    int getQuantidadeDeContatos();
    Perfil** getContatos();
    list<Postagem*>* getPostagens();
    list<Postagem*>* getPostagensDosContatos();


    int getId();
    static int getUltimoId();

};

#endif

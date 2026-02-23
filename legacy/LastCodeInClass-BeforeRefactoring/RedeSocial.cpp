#include "RedeSocial.h"
#include "Perfil.h"
#include "PessoaVerificada.h"
#include "Pessoa.h"
#include "Pagina.h"
#include "PerfilInexistente.h"
#include <fstream>

int RedeSocial::getQuantidadeDePerfis() {
  return this->quantidadeDePerfis;
}

Perfil** RedeSocial::getPerfis() {
  return perfis;
}

Perfil* RedeSocial::getPerfil(int id) {
  for (int i=0; i < this->quantidadeDePerfis; i++) {
    if (this->perfis[i]->getId() == id) {
      return perfis[i];
    }
  }
  throw new PerfilInexistente();
}

RedeSocial::RedeSocial (){
  perfis = new Perfil*[capacidade];
};

bool RedeSocial::adicionar (Perfil* Perfil) {
  if (this->quantidadeDePerfis < capacidade) {
    perfis[quantidadeDePerfis] = Perfil;
    quantidadeDePerfis += 1;
    return true;
  }
  return false;
}

RedeSocial::~RedeSocial() {
  cout << "Destrutor de RedeSocial: " << quantidadeDePerfis << " perfis" << endl;
  	for (int i = 0; i < quantidadeDePerfis; i++) {
        delete perfis[i];
    }
    delete[] perfis;
  cout << "RedeSocial deletada " << endl;
}

void RedeSocial::imprimir () {
  cout << "==================================" << endl;
  cout << "Rede Social: " << quantidadeDePerfis << " perfis" << endl;
  cout << "==================================" << endl;
  if (quantidadeDePerfis == 0){
    cout << "Sem perfis" << endl;
    cout << "==================================" << endl;
  } else {
    for (int i = 0; i < quantidadeDePerfis; i++){
      perfis[i]->imprimir();
      cout << "==================================" << endl;
    }
  }
  cout << endl;
}


void RedeSocial::imprimirEstatisticas() {
  int quantidadePessoasVerificadas = 0;
  int quantidadePessoas = 0;
  int quantidadePaginas = 0;
  int quantidadePerfis = 0;
  for (int i = 0; i < this->quantidadeDePerfis; i++) {
    if (dynamic_cast<PessoaVerificada*>(perfis[i]) != NULL) {
        quantidadePessoasVerificadas++;
    }
    else if (dynamic_cast<Pessoa*>(perfis[i]) != NULL) {
        quantidadePessoas++;
    }
    else if (dynamic_cast<Pagina*>(perfis[i]) != NULL) {
        quantidadePaginas++;
    }
    else {
        quantidadePerfis++;
    }
  }

  cout << "PessoaVerificada: " << quantidadePessoasVerificadas << endl;
  cout << "PessoaNaoVerificada: " << quantidadePessoas << endl;
  cout << "Pagina: " << quantidadePaginas << endl;
  cout << "Perfil: " << quantidadePerfis << endl;
}





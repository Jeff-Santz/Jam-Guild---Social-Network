#ifndef REDESOCIAL_H
#define REDESOCIAL_H

#include "Perfil.h"
using namespace std;

class RedeSocial {
public:
  RedeSocial();
  ~RedeSocial();

  Perfil** getPerfis();
  Perfil* getPerfil(int id); 
  int getQuantidadeDePerfis();
  bool adicionar(Perfil* perfil);
  void imprimir();
  void imprimirEstatisticas();

private:
  Perfil** perfis;
  const int capacidade = 100;
  int quantidadeDePerfis = 0;
};


#endif
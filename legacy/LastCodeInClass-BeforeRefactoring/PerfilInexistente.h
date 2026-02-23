#include "Perfil.h"
#include <iostream>
#include <string>
#include <stdexcept>
using namespace std;

class PerfilInexistente : public logic_error {
public:
  PerfilInexistente ();
};
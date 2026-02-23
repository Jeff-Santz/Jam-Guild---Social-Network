#include "RedeSocial.h"
#include <iostream>
#include <string>
#include <stdexcept>
using namespace std;

class PersistenciaDaRede {
public:
    PersistenciaDaRede(string arquivo);
    virtual ~PersistenciaDaRede();

    void salvar(RedeSocial* r);
    RedeSocial* carregar();

private:
    string arquivo;
};
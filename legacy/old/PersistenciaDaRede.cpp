// #include "PersistenciaDaRede.h"
// #include "PessoaVerificada.h"
// #include "RedeSocial.h"
// #include <iostream>
// #include <string>
// #include <fstream>
// using namespace std;


// PersistenciaDaRede::PersistenciaDaRede(string arquivo) {
//     this->arquivo = arquivo;
// };

// PersistenciaDaRede::~PersistenciaDaRede() {}

// void PersistenciaDaRede::salvar(RedeSocial* r) {
//     ofstream escreva;
//     escreva.open (this->arquivo);
//     if (escreva.fail()) {
//         throw logic_error("Erro ao escrever no arquivo");
//     }

//     // Pegando a Quantidade De Pessoas Verificadas
//     int quantidadePessoasVerificadas = 0;
//     for (int i = 0; i < r->getQuantidadeDePerfis(); i++) {
//         if (dynamic_cast<PessoaVerificada*>(r->getPerfis()[i]) != NULL) {
//             quantidadePessoasVerificadas++;
//         }
//     }
//     escreva << quantidadePessoasVerificadas << endl;

//     // Escrevendo Pra cada Pessoa Verificada
//     for (int i = 0; i < r->getQuantidadeDePerfis(); i++) {
//         PessoaVerificada* pv = dynamic_cast<PessoaVerificada*>(r->getPerfis()[i]);
//         if (pv != NULL) {
//             escreva << pv->getId()   << " "
//                     << pv->getNome() << " "
//                     << pv->getEmail() << endl;
//         }
//     }
//     escreva.close();
// }

// RedeSocial* PersistenciaDaRede::carregar() {
//     ifstream leia;
//     leia.open(this->arquivo);
//     if (leia.fail() ) {
//         throw logic_error("Erro ao ler o arquivo");
//     }

//     int quantidade;
//     leia >> quantidade;

//     RedeSocial* Facebook_Carregado = new RedeSocial();

//     for (int i = 0; i < quantidade; i++) {
//         int id_ignorado;
//         string nome, email;

//         leia >> id_ignorado >> nome >> email;

//         if (!leia.good()) {
//             throw logic_error("Erro ao ler dados da pessoa");
//         }

//         PessoaVerificada* Pessoa_Atual = new PessoaVerificada(nome, email, 10);
//         Facebook_Carregado->adicionar(Pessoa_Atual);
//     }
//     return Facebook_Carregado;
// }
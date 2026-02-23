#include "Perfil.h"
#include "Postagem.h"
#include "RedeSocial.h"
#include "PessoaVerificada.h"
#include "Pessoa.h"
#include "Pagina.h"
#include "PersistenciaDaRede.h"
#include <iostream>
#include <string>
#include <stdexcept>
#include <list>

using namespace std;


void imprimirRede (Perfil** perfis, int quantidade) {
    cout << "==================================" << endl;
    for (int i = 0; i < quantidade; i++) {
        perfis[i]->imprimir();
    }
}

void teste() {
    // a) Pessoa Verificada Marcelo
    PessoaVerificada* Marcelo = new PessoaVerificada("Marcelo", "m@usp.br", 10);

    // b) Pessoa Maria
    Pessoa* Maria = new Pessoa("Maria", 10);

    // c) Página PCS3111 com responsável Marcelo
    Pagina* PCS3111 = new Pagina("PCS3111", Marcelo, 10);

    // d) RedeSocial
    RedeSocial* Facebook = new RedeSocial();

    // e) Adicionar os três objetos na ordem de criação
    Facebook->adicionar(Marcelo);
    Facebook->adicionar(Maria);
    Facebook->adicionar(PCS3111);

    // f) Maria se torna contato de Marcelo e da página PCS3111
    Marcelo->adicionarContato(Maria);  
    PCS3111->adicionarContato(Maria);    

    // g) Cada objeto publica uma postagem
    Postagem* m1 = new Postagem("Mensagem 1", 1, Marcelo);
    Marcelo->adicionarPostagem(m1);

    Postagem* m2 = new Postagem("Mensagem 2", 2, Maria);
    Maria->adicionarPostagem(m2);

    Postagem* m3 = new Postagem("Mensagem 3", 3, PCS3111);
    PCS3111->adicionarPostagem(m3);

    // h) Imprimir Postagens dos contatos da Maria
    list<Postagem*>* lista = Maria->getPostagensDosContatos();

    for (Postagem* p : *lista) {
        p->imprimir();
    }

    // i) Destruir RedeSocial (isso deve destruir os perfis também)
    delete Facebook;
}


// int main() {
// 	teste();
// 	return 0;
// }

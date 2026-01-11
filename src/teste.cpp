#include <iostream>
#include "SocialNetwork.h"
#include "User.h"
#include "VerifiedUser.h"
#include "Page.h"
#include "Post.h"

using namespace std;

int main() {
    cout << "=== STARTING SOCIAL NETWORK TEST ===" << endl;

    // 1. Criar a Rede
    SocialNetwork sn; 

    // 2. Criar Perfis (Sem carregar de arquivo, criando na raça)
    // Lembre-se: seus construtores pedem (nome, max_contatos) ou algo assim
    // Ajuste os parametros conforme o seu User.h / Profile.h
    User* jeff = new User("Jeff", 10); 
    User* lapis = new User("Lapis", 10);
    
    // Perfil Verificado
    VerifiedUser* marcelo = new VerifiedUser("Prof. Marcelo", "marcelo@usp.br", 20);
    
    // Pagina (Dono é o Marcelo)
    Page* poli = new Page("Poli USP", marcelo, 50);

    // 3. Adicionar na Rede
    cout << "Adding profiles..." << endl;
    sn.add(jeff);
    sn.add(lapis);
    sn.add(marcelo);
    sn.add(poli);

    // 4. Criar Conexões (Jeff segue Lapis)
    cout << "Connecting people..." << endl;
    jeff->addContact(lapis);
    // lapis->addContact(jeff); // Se sua lógica for bidirecional automática, não precisa dessa linha

    // 5. Criar Posts
    cout << "Posting..." << endl;
    // Ajuste a data conforme seu Post.h (int ou string)
    Post* p1 = new Post("Refatorando tudo para C++ Moderno!", 20260112, jeff);
    jeff->addPost(p1);

    Post* p2 = new Post("Aulas de POO retornam amanha.", 20260112, marcelo);
    marcelo->addPost(p2);

    // 6. Testar o Relatório
    cout << "\n=== STATUS DA REDE ===" << endl;
    sn.print();
    sn.printStatistics();

    cout << "\n=== BUSCA POR ID ===" << endl;
    try {
        Profile* p = sn.getProfile(jeff->getId());
        cout << "Found: " << p->getName() << endl;
    } catch (exception& e) {
        cout << "Error: " << e.what() << endl;
    }

    // O destrutor da SocialNetwork deve limpar a memória dos perfis
    // Mas os posts precisam ser gerenciados se não estiverem na árvore de delete do Profile
    cout << "\n=== END OF TEST ===" << endl;
    return 0;
}
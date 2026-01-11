#include <iostream>
#include <limits>
#include "SocialNetwork.h"
#include "NetworkStorage.h"
#include "User.h"
#include "VerifiedUser.h"
#include "Page.h"

using namespace std;

void cleanBuffer() { cin.ignore(numeric_limits<streamsize>::max(), '\n'); }

int main() {
    SocialNetwork sn;
    NetworkStorage storage("rede_social.db");
    
    // Carrega tudo
    storage.load(&sn);

    Profile* currentUser = nullptr; // <--- SESSÃO DO USUARIO
    int option = -1;

    while (option != 0) {
        cout << "\n===============================" << endl;
        if (currentUser == nullptr) {
            // --- MENU DE VISITANTE ---
            cout << "Bem-vindo a Rede Social!" << endl;
            cout << "1. Fazer Login" << endl;
            cout << "2. Criar Nova Conta" << endl;
            cout << "0. Sair" << endl;
        } else {
            // --- MENU DE USUARIO LOGADO ---
            cout << "Ola, " << currentUser->getName() << "!" << endl;
            cout << "1. Ver Timeline" << endl;
            cout << "2. Fazer Postagem" << endl;
            cout << "3. Buscar/Adicionar Amigo" << endl;
            cout << "4. Logout (Sair da conta)" << endl;
            cout << "0. Sair do Sistema" << endl;
        }
        cout << "Escolha: ";
        cin >> option;
        cleanBuffer();

        if (currentUser == nullptr) {
            // Lógica de Visitante
            if (option == 1) {
                string nome, senha;
                cout << "Usuario: "; getline(cin, nome);
                cout << "Senha: ";   getline(cin, senha);
                
                currentUser = sn.login(nome, senha);
                if (currentUser) cout << "Login realizado com sucesso!" << endl;
                else cout << "Usuario ou senha invalidos!" << endl;

            } else if (option == 2) {
                string nome, senha;
                cout << "Escolha seu Usuario: "; getline(cin, nome);
                cout << "Escolha sua Senha: ";   getline(cin, senha);
                
                // Criação simples (User padrão)
                User* u = new User(nome, senha);
                sn.add(u);
                cout << "Conta criada! Faca login para usar." << endl;
                storage.save(&sn); // Salva logo pra garantir
            } 
            else if (option != 0) cout << "Opcao invalida." << endl;

        } else {
            // Lógica de Usuário Logado
            switch (option) {
                case 1: 
                    sn.print(); // Futuramente mudar para showFeed()
                    break;
                case 2: {
                    string txt;
                    cout << "O que voce esta pensando? ";
                    getline(cin, txt);
                    // Data hardcoded por enquanto, vamos mudar no proximo passo
                    currentUser->addPost(new Post(txt, 20260112, currentUser));
                    cout << "Postado!" << endl;
                    storage.save(&sn); // Auto-save
                    break;
                }
                case 3:
                    cout << "Recurso em desenvolvimento (Busca)..." << endl;
                    break;
                case 4:
                    currentUser = nullptr;
                    cout << "Voce saiu da conta." << endl;
                    break;
                case 0:
                    cout << "Salvando e fechando..." << endl;
                    storage.save(&sn);
                    break;
                default:
                    cout << "Opcao invalida." << endl;
            }
        }
    }
    return 0;
}
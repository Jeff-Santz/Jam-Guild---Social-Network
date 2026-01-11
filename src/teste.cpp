#include <iostream>
#include <limits> // Para limpar o buffer do cin
#include "SocialNetwork.h"
#include "User.h"
#include "VerifiedUser.h"
#include "Page.h"
#include "Post.h"
#include "NetworkStorage.h"

using namespace std;

// Funcao auxiliar para limpar o ENTER que sobra no teclado
void cleanBuffer() {
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

int main() {
    SocialNetwork sn;
    NetworkStorage storage("rede_social.db");

    // 1. CARREGAR DADOS EXISTENTES
    cout << "Inicializando sistema..." << endl;
    storage.load(&sn);
    cout << "Carregados " << sn.getProfilesAmount() << " perfis do banco de dados." << endl;

    int option = -1;
    
    // 2. LOOP DO MENU (O programa não fecha sozinho)
    while (option != 0) {
        cout << "\n=== REDE SOCIAL DO JEFF ===" << endl;
        cout << "1. Mostrar Timeline (Ver tudo)" << endl;
        cout << "2. Adicionar Novo Usuario" << endl;
        cout << "3. Fazer uma Postagem" << endl;
        cout << "4. Salvar Agora" << endl;
        cout << "0. Salvar e Sair" << endl;
        cout << "Escolha: ";
        cin >> option;
        cleanBuffer(); // Limpa o enter

        switch (option) {
            case 1:
                sn.print();
                break;

            case 2: {
                cout << "Nome do Usuario: ";
                string nome;
                getline(cin, nome);
                
                User* u = new User(nome);
                sn.add(u);
                cout << "Usuario " << nome << " criado com ID " << u->getId() << endl;
                break;
            }

            case 3: {
                cout << "Digite o ID de quem vai postar: ";
                int id;
                cin >> id;
                cleanBuffer();

                try {
                    Profile* autor = sn.getProfile(id);
                    
                    cout << "Escreva o post: ";
                    string texto;
                    getline(cin, texto);

                    // Data fixa por enquanto (depois podemos automatizar)
                    Post* p = new Post(texto, 20260112, autor);
                    autor->addPost(p);
                    cout << "Post publicado!" << endl;

                } catch (...) {
                    cout << "ERRO: Usuario com ID " << id << " nao existe!" << endl;
                }
                break;
            }

            case 4:
                storage.save(&sn);
                cout << "Dados salvos no disco!" << endl;
                break;

            case 0:
                cout << "Salvando antes de sair..." << endl;
                storage.save(&sn);
                cout << "Ate logo!" << endl;
                break;

            default:
                cout << "Opcao invalida!" << endl;
        }
    }

    return 0;
}
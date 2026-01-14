#include <iostream>
#include "SocialNetwork.h"
#include "NetworkStorage.h"
#include "UIHandlers.h"
#include "ContactHandlers.h"
#include "Utils.h"

int main() {
    // 1. Inicialização do Sistema
    SocialNetwork sn;
    NetworkStorage storage("rede_social.db");
    
    // 2. Carga inicial de dados
    try {
        storage.load(&sn);
    } catch (const std::exception& e) {
        std::cerr << ">> Aviso: Iniciando com banco de dados vazio." << std::endl;
    }

    Profile* currentUser = nullptr;
    Page* currentPage = nullptr;
    int option = -1;

    // 3. Loop Principal
    while (option != 0) {
        system("clear || cls"); // Mantém o terminal limpo

        // --- CENÁRIO 1: NINGUÉM LOGADO ---
        if (currentUser == nullptr) {
            option = UIHandlers::showVisitorMenu();

            if (option == 1) {
                UIHandlers::handleLogin(&sn, currentUser);
                if (currentUser) {
                    std::cout << "Pressione Enter para ir ao Dashboard...";
                    getchar();
                }
            } else if (option == 2) {
                UIHandlers::handleCreateAccount(&sn, storage);
                std::cout << "Pressione Enter para continuar...";
                getchar();
            }
        } 
        
        // --- CENÁRIO 2: USUÁRIO LOGADO ---
        else if (currentPage == nullptr) {
            UIHandlers::showNotifications(currentUser);
            option = UIHandlers::showUserMenu(currentUser);

            switch (option) {
                case 1: UIHandlers::handleViewProfile(currentUser); break;
                case 2: UIHandlers::handleCreatePost(currentUser, storage, sn); break;
                case 3: UIHandlers::handleSearchUsers(currentUser, sn, storage); break;
                case 4: UIHandlers::handleTimeline(currentUser, sn, storage); break;
                case 5: UIHandlers::handleEditProfile(currentUser, storage, sn); break;
                case 6: Utils::showHeader("CENTRAL DE AMIZADES");
                    std::cout << "1) Ver Meus Contatos\n";
                    std::cout << "2) Enviar Pedido (Busca)\n";
                    std::cout << "3) Gerenciar Pedidos Recebidos\n";
                    std::cout << "4) Remover Contato\n";
                    std::cout << "5) Visitar Perfil de Amigo\n"; 
                    std::cout << "0) Voltar\nEscolha: ";
                    
                    int cOp; std::cin >> cOp; Utils::cleanBuffer();
                    
                    if (cOp == 1) ContactHandlers::handleListContacts(currentUser);
                    else if (cOp == 2) UIHandlers::handleSearchUsers(currentUser, sn, storage);
                    else if (cOp == 3) ContactHandlers::handleManageRequests(currentUser, sn, storage);
                    else if (cOp == 4) ContactHandlers::handleRemoveContact(currentUser, sn, storage);
                    else if (cOp == 5) ContactHandlers::handleInspectFriend(currentUser, sn); // <-- E ESTA
                    break;
                    case 7: UIHandlers::handleVerify(currentUser, sn, storage); break;
                    case 8: UIHandlers::handleCreatePage(currentUser, sn, storage); break;
                    case 9: UIHandlers::handleManageMyPages(currentUser, sn, currentPage); break;
                    case 0: 
                        currentUser = nullptr; 
                        break;
                }
            if (option != 0) {
                std::cout << "\n[Pressione Enter para continuar]";
                getchar();
            }
        }
        
        // --- CENÁRIO 3: GERENCIANDO PÁGINA ---
        else {
            option = UIHandlers::showPageMenu(currentPage);
            switch (option) {
                case 1: UIHandlers::handleCreatePost(currentPage, storage, sn); break;
                case 2: ContactHandlers::handleListContacts(currentPage); break;
                case 3: UIHandlers::handleEditProfile(currentPage, storage, sn); break;
                case 0: currentPage = nullptr; break;
            }
        }
    }

    // 4. Salvar antes de fechar
    storage.save(&sn);
    std::cout << "Sistema encerrado e dados salvos com sucesso!" << std::endl;

    return 0;
}
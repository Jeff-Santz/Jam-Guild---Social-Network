#include <iostream>
#include <string>
#include "ContactHandlers.h"
#include "Utils.h"

namespace ContactHandlers {
    void acceptFriendRequest(Profile* currentUser, Profile* requester, SocialNetwork& sn, NetworkStorage& storage) {
    if (!currentUser || !requester) return;

    currentUser->addContact(requester);
    currentUser->removeRequest(requester);

    User* uRequester = dynamic_cast<User*>(requester);
    if (uRequester) {
        std::string msg = currentUser->getName() + " aceitou seu pedido de amizade!";
        uRequester->addNotification(new Notification(msg));
    }
    storage.save(&sn);
}

void refuseFriendRequest(Profile* currentUser, Profile* requester, SocialNetwork& sn, NetworkStorage& storage) {
    if (!currentUser || !requester) return;

    currentUser->removeRequest(requester);
    storage.save(&sn);
}

void handleAddContact(Profile* currentUser, SocialNetwork& sn, NetworkStorage& storage) {
    if (!currentUser) return;

    std::string target;
    std::cout << "Digite o nome exato do perfil: ";
    std::getline(std::cin, target);

    std::vector<Profile*> results = sn.searchProfiles(target);
    
    if (results.empty()) {
        std::cout << "Ninguem encontrado com esse nome.\n";
        return;
    }

    Profile* p = results[0]; // Pega o primeiro resultado

    if (p == currentUser) {
        std::cout << "Voce nao pode adicionar voce mesmo.\n";
        return;
    }

    try {
        currentUser->addContact(p);
        storage.save(&sn); // CORRIGIDO: adicionado o &
        std::cout << p->getName() << " foi adicionado aos contatos!\n";
    } catch (const std::exception& e) {
        std::cout << "[!] Erro: " << e.what() << "\n";
    }
}

void handleListContacts(Profile* currentUser) {
    if (!currentUser) {
        std::cout << "Nenhum usuario logado.\n";
        return;
    }

    auto contacts = currentUser->getContacts();
    if (contacts->empty()) {
        std::cout << "Voce nao tem contatos ainda.\n";
        return;
    }

    std::cout << "Seus contatos:\n";
    for (size_t i = 0; i < contacts->size(); i++) {
        std::cout << i << ") " << (*contacts)[i]->getName() << "\n";
    }
}

void handleRemoveContact(Profile* currentUser, SocialNetwork& sn, NetworkStorage& storage) {
    if (!currentUser) {
        std::cout << ">> Erro: Nenhum usuario logado.\n";
        return;
    }

    auto* contacts = currentUser->getContacts();

    if (!contacts || contacts->empty()) {
        std::cout << "\n>> Voce nao tem contatos para remover.\n";
        std::cout << "[Pressione Enter para voltar]";
        getchar();
        return;
    }

    Utils::showHeader("REMOVER CONTATO");
    
    for (size_t i = 0; i < contacts->size(); i++) {
        std::cout << i << ") " << (*contacts)[i]->getName() 
                  << " [" << (*contacts)[i]->getSubtitle() << "]\n";
    }
    std::cout << "-1) Cancelar\n";
    std::cout << "---------------------------------------\n";
    std::cout << "Escolha o numero do contato: ";

    int choice;
    if (!(std::cin >> choice)) {
        std::cin.clear();
        std::cin.ignore(1000, '\n');
        std::cout << ">> Erro: Entrada invalida. Digite um numero.\n";
        return;
    }
    Utils::cleanBuffer();

    if (choice == -1) return;

    if (choice < 0 || choice >= (int)contacts->size()) {
        std::cout << ">> Erro: Opcao fora do intervalo.\n";
        return;
    }

    Profile* target = (*contacts)[choice];
    std::string removedName = target->getName();

    std::cout << "Tem certeza que deseja remover " << removedName << "? (s/n): ";
    char confirm;
    std::cin >> confirm;
    Utils::cleanBuffer();

    if (confirm == 's' || confirm == 'S') {
       
        currentUser->removeContact(target);
        storage.save(&sn);
        
        std::cout << ">> Sucesso: " << removedName << " foi removido da sua rede.\n";
    } else {
        std::cout << ">> Operacao cancelada.\n";
    }
}

void handleSendRequest(Profile* currentUser, SocialNetwork& sn, NetworkStorage& storage) {
    if (!currentUser) return;

    try {
        std::cout << "Digite o nome da pessoa: ";
        std::string targetName;
        std::getline(std::cin, targetName);

        std::vector<Profile*> results = sn.searchProfiles(targetName);
        
        if (results.empty()) {
            throw std::invalid_argument("Perfil nao encontrado com este nome.");
        }

        Utils::showHeader("RESULTADOS DA BUSCA");
        for (size_t i = 0; i < results.size(); i++) {
            std::cout << i << ") " << results[i]->getName() << " (@" << results[i]->getId() << ")\n";
        }
    
        std::cout << "\nEscolha o numero (ou -1 para cancelar): ";
        int choice;
        if (!(std::cin >> choice)) {
            Utils::cleanBuffer();
            throw std::invalid_argument("Entrada invalida. Digite um numero.");
        }
        Utils::cleanBuffer();

        if (choice == -1) return;
        if (choice < 0 || choice >= (int)results.size()) {
            throw std::invalid_argument("Opcao de perfil invalida.");
        }

        Profile* target = results[choice];

        if (target == currentUser) {
            throw std::invalid_argument("Voce nao pode enviar um pedido para si mesmo.");
        }

        // 1. Checar se já são amigos para evitar pedidos redundantes
        auto* currentContacts = currentUser->getContacts();
        for (auto* contact : *currentContacts) {
            if (contact == target) {
                throw std::invalid_argument("Voces ja sao amigos!");
            }
        }

        // 2. Lógica de Auto-match se o outro já me pediu
        bool theyAlreadyRequestedMe = false;
        auto& myRequests = currentUser->getContactRequests();
        
        for (auto* req : myRequests) {
            if (req == target) {
                theyAlreadyRequestedMe = true;
                break;
            }
        }

        if (theyAlreadyRequestedMe) {
            // Se o alvo já me pediu, aceitamos automaticamente
            currentUser->addContact(target); 
            currentUser->removeRequest(target); 
            
            std::cout << "\n>> Match! " << target->getName() << " ja tinha te enviado um pedido.\n";
            std::cout << ">> Voces agora estao conectados!\n";
            
            User* uTarget = dynamic_cast<User*>(target);
            if (uTarget) {
                std::string msg = currentUser->getName() + " aceitou seu pedido!";
                uTarget->addNotification(new Notification(msg));
            }
        } 
        else {
            // Fluxo normal de pedido de amizade
            target->addContactRequest(currentUser);
            
            User* uTarget = dynamic_cast<User*>(target);
            if (uTarget) {
                std::string msg = currentUser->getName() + " enviou um pedido de amizade!";
                uTarget->addNotification(new Notification(msg));
            }
            std::cout << ">> Sucesso: Pedido de amizade enviado para " << target->getName() << "!\n";
        }

        // 3. Salva a rede social após a alteração de estado
        storage.save(&sn);

    } catch (const std::invalid_argument& e) {
        std::cout << "\n[!] Erro: " << e.what() << "\n";
    } catch (const std::exception& e) {
        std::cout << "\n[!] Erro inesperado: " << e.what() << "\n";
    }
}

void handleManageRequests(Profile* currentUser, SocialNetwork& sn, NetworkStorage& storage) {
    if (!currentUser) return;

    auto& requests = currentUser->getContactRequests();

    if (requests.empty()) {
        std::cout << "\n>> Voce nao tem pedidos de amizade pendentes.\n";
        return;
    }

    Utils::showHeader("PEDIDOS RECEBIDOS");
    for (size_t i = 0; i < requests.size(); i++) {
        std::cout << i << ") " << requests[i]->getName() 
                  << " (" << requests[i]->getSubtitle() << ")\n";
    }
    std::cout << "-1) Voltar\n";
    std::cout << "Escolha um pedido para gerenciar: ";

    int choice;
    if (!(std::cin >> choice)) {
        std::cin.clear();
        std::cin.ignore(1000, '\n');
        return;
    }
    Utils::cleanBuffer();

    if (choice == -1) return;

    if (choice >= 0 && choice < (int)requests.size()) {
        Profile* sender = requests[choice];

        std::cout << "\nO que deseja fazer com o pedido de " << sender->getName() << "?\n";
        std::cout << "1. Aceitar\n2. Recusar\n0. Voltar\nEscolha: ";
        
        int op;
        std::cin >> op;
        Utils::cleanBuffer();

        if (op == 1) {
            ContactHandlers::acceptFriendRequest(currentUser, sender, sn, storage);
            std::cout << ">> Pedido aceito!\n";
            storage.save(&sn);
        } 
        else if (op == 2) {
            ContactHandlers::refuseFriendRequest(currentUser, sender, sn, storage);
            std::cout << ">> Pedido recusado.\n";
            storage.save(&sn);
        }
    }
}

void handleInspectFriend(Profile* currentUser, SocialNetwork& sn) {
    auto* myContacts = currentUser->getContacts();
    if (!myContacts || myContacts->empty()) {
        std::cout << "\n>> Voce ainda nao tem amigos para inspecionar.\n";
        return;
    }

    Utils::showHeader("VISITAR AMIGO");
    for (size_t i = 0; i < myContacts->size(); i++) {
        std::cout << i << ") " << (*myContacts)[i]->getName() << "\n";
    }
    std::cout << "-1) Voltar\nEscolha: ";
    
    int choice;
    if (!(std::cin >> choice) || choice == -1) {
        std::cin.clear();
        std::cin.ignore(1000, '\n');
        return;
    }
    Utils::cleanBuffer();

    if (choice >= 0 && choice < (int)myContacts->size()) {
        Profile* friendPtr = (*myContacts)[choice];
        
        Utils::showHeader("PERFIL DE " + friendPtr->getName());
        friendPtr->print(); 

        std::cout << "\n1) Ver amigos de " << friendPtr->getName();
        std::cout << "\n0) Voltar\nEscolha: ";
        
        int op;
        std::cin >> op; Utils::cleanBuffer();

        if (op == 1) {
            auto* theirContacts = friendPtr->getContacts();
            std::cout << "\n--- CONEXOES DE " << friendPtr->getName() << " ---\n";
            
            if (!theirContacts || theirContacts->empty()) {
                std::cout << "Este usuario ainda nao possui conexoes.\n";
            } else {
                std::vector<Profile*> strangers; 
                
                for (auto* f : *theirContacts) {
                    bool isAlreadyFriend = false;
                    for(auto* myF : *myContacts) if(myF == f) isAlreadyFriend = true;

                    std::string label = "";
                    if (f == currentUser) label = " (Voce)";
                    else if (isAlreadyFriend) label = " (Amigo)";
                    else {
                        label = " [" + std::to_string(strangers.size()) + "] -> Sugestao";
                        strangers.push_back(f);
                    }

                    std::cout << "- " << f->getName() << label << "\n";
                }

                if(!strangers.empty()) {
                    std::cout << "\nDigite o numero entre [] para enviar pedido ou -1 para sair: ";
                    int addId;
                    std::cin >> addId; Utils::cleanBuffer();
                    if(addId >= 0 && addId < (int)strangers.size()) {
                        strangers[addId]->addContactRequest(currentUser);
                        std::cout << ">> Pedido enviado para " << strangers[addId]->getName() << "!\n";
                    }
                }
            }
            std::cout << "--------------------------------\n";
            std::cout << "[Pressione Enter para voltar]";
            getchar();
        }
    }
}

}

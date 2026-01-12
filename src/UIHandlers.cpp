#include "UIHandlers.h"
#include "Utils.h"
#include "ContactHandlers.h"
#include "ImagePost.h"
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>


namespace UIHandlers {

int showVisitorMenu() {
    Utils::showHeader("Menu Visitante");
    std::cout << "1) Login\n";
    std::cout << "2) Criar Conta\n";
    std::cout << "0) Sair\n";
    
    int opc;
    std::cin >> opc;
    Utils::cleanBuffer();
    return opc;
}

int showUserMenu(Profile* user) {
    std::string status = "";
    User* uPtr = dynamic_cast<User*>(user);
    if (uPtr && uPtr->isVerified()) status = " [VERIFICADO]";

    Utils::showHeader("Menu do Usuario: " + user->getName() + status);
    std::cout << "1) Ver Perfil\n";
    std::cout << "2) Criar Postagem\n";
    std::cout << "3) Buscar Usuarios / Paginas\n";
    std::cout << "4) Linha do Tempo\n";
    std::cout << "5) Editar Perfil\n";
    std::cout << "6) Contatos (Amigos)\n";
    std::cout << "7) Solicitar Verificacao\n"; 
    std::cout << "8) Criar Nova Pagina\n";      
    std::cout << "9) Gerenciar Minhas Paginas\n"; 
    std::cout << "0) Logout\n";
    
    int opc;
    if (!(std::cin >> opc)) {
        Utils::cleanBuffer();
        return -1;
    }
    Utils::cleanBuffer();
    return opc;
}

void showNotifications(Profile* user) {
    User* uPtr = dynamic_cast<User*>(user);
    if (!uPtr) return;

    auto& notifications = uPtr->getNotifications();

    if (!notifications.empty()) {
        std::cout << "\n=== NOTIFICACOES ===\n";
        for (const auto& n : uPtr->getNotifications()) {
            std::cout << "[" << n->getFormattedDate() << "] " << n->getMessage() << "\n";
        }
        std::cout << "====================\n";
        
        // Limpa as notificações após serem lidas
        uPtr->clearNotifications(); 
        std::cout << "[Pressione Enter para prosseguir]";
        getchar();
    }
}

int showPageMenu(Page* page) {
    Utils::showHeader("PAINEL DA PAGINA: " + page->getName());
    std::cout << "Dono: " << page->getOwner()->getName() << "\n";
    std::cout << "-----------------------------------\n";
    std::cout << "1) Criar Postagem da Pagina\n";
    std::cout << "2) Ver Seguidores (Contatos)\n";
    std::cout << "3) Editar Informacoes da Pagina\n";
    std::cout << "0) Sair do Painel\n";
    std::cout << "Escolha: ";

    int op;
    std::cin >> op; Utils::cleanBuffer();
    return op;
}

void handleLogin(SocialNetwork* sn, Profile*& currentUser) {
    try {
        int id;
        std::string pass;

        std::cout << "ID de Usuario: ";
        if (!(std::cin >> id)) {
            std::cin.clear();
            std::cin.ignore(1000, '\n');
            throw std::invalid_argument("O ID deve ser um numero inteiro.");
        }
        Utils::cleanBuffer();

        std::cout << "Senha: ";
        std::getline(std::cin, pass);

        Profile* p = sn->login(id, pass);
        
        if (!p) {
            throw std::invalid_argument("ID ou Senha invalidos!");
        }

        currentUser = p;
        std::cout << ">> Login realizado com sucesso!\n";

    } catch (const std::invalid_argument& e) {
        std::cout << ">> Erro de Login: " << e.what() << "\n";
    }
}

void handleCreateAccount(SocialNetwork* sn, NetworkStorage& storage) {
    std::string nome, senha;

    std::cout << "Nome de Usuario: ";
    std::getline(std::cin, nome);
    std::cout << "Senha: ";
    std::getline(std::cin, senha);

    User* newUser = new User(nome, senha);
    sn->add(newUser);
    
    storage.save(sn);

    Utils::showHeader("CONTA CRIADA COM SUCESSO");
    std::cout << "ATENCAO: Seu nome e repetivel, entao use seu ID para entrar.\n";
    std::cout << "--------------------------------------------------\n";
    std::cout << " SEU ID DE ACESSO E: " << newUser->getId() << "\n";
    std::cout << " GUARDE ESTE NUMERO EM UM LUGAR SEGURO!\n";
    std::cout << "--------------------------------------------------\n";
}

void handleViewProfile(Profile* user) {
    Utils::showHeader("Perfil");
    user->print();
}

void handleCreatePost(Profile* user, NetworkStorage& storage, SocialNetwork& sn) {
    std::string txt;
    std::cout << "O que voce esta pensando? ";
    std::getline(std::cin, txt);

    std::cout << "Deseja adicionar uma imagem? (s/n): ";
    char resp;
    std::cin >> resp;
    Utils::cleanBuffer();

    if (resp == 's' || resp == 'S') {
        std::cout << "Caminho da imagem: ";
        std::string path;
        std::getline(std::cin, path);
        user->addPost(new ImagePost(txt, user, path));
    } else {
        user->addPost(new Post(txt, user));
    }

    storage.save(&sn); // Passando o endereço corretamente
    std::cout << ">> Postagem publicada!\n";
}

void handleCreatePage(Profile* currentUser, SocialNetwork& sn, NetworkStorage& storage) {
    User* uPtr = dynamic_cast<User*>(currentUser);
    if (!uPtr) return;

    Utils::showHeader("CRIAR NOVA PAGINA");
    std::cout << "Nome da Pagina: ";
    std::string pName;
    std::getline(std::cin, pName);

    std::cout << "Categoria (ex: Banda, Empresa, Blog): ";
    std::string category;
    std::getline(std::cin, category);

    std::cout << "Senha da Pagina: ";
    std::string pPass;
    std::getline(std::cin, pPass);

    // Cria o objeto Page passando o usuário logado como Owner
    Page* newPage = new Page(pName, uPtr, pPass);
    newPage->setSubtitle(category);
    
    sn.add(newPage);
    storage.save(&sn);

    std::cout << ">> Pagina '" << pName << "' criada! ID para login: " << newPage->getId() << "\n";
}

void handleManageMyPages(Profile* currentUser, SocialNetwork& sn, Page*& currentPage) {
    User* uPtr = dynamic_cast<User*>(currentUser);
    if (!uPtr) return;

    std::vector<Page*> myPages = sn.getPagesByOwner(uPtr->getId());

    Utils::showHeader("MINHAS PAGINAS");
    if (myPages.empty()) {
        std::cout << "Voce ainda nao possui nenhuma pagina.\n";
        return;
    }

    for (size_t i = 0; i < myPages.size(); i++) {
        std::cout << i << ") " << myPages[i]->getName() 
                  << " [ID: " << myPages[i]->getId() << "]\n";
    }

    std::cout << "\nDigite o numero para gerenciar a pagina (ou -1 para voltar): ";
    int choice;
    std::cin >> choice; Utils::cleanBuffer();

    if (choice >= 0 && choice < (int)myPages.size()) {
        currentPage = myPages[choice]; // Define a página atual para o Loop do Main mudar de cenário
        std::cout << ">> Alternando para o painel da pagina: " << currentPage->getName() << "\n";
    }
}

void handleEditProfile(Profile* user, NetworkStorage& storage, SocialNetwork& sn) {
    Utils::showHeader("EDITAR PERFIL");
    std::string s;

    std::cout << "Bio atual: " << user->getBio() << "\n";
    std::cout << "Nova Bio (deixe em branco para manter): ";
    std::getline(std::cin, s);
    if(!s.empty()) user->setBio(s);

    std::cout << "Genero atual: " << user->getSubtitle() << "\n";
    std::cout << "Novo Genero: ";
    std::getline(std::cin, s);
    if(!s.empty()) user->setSubtitle(s);

    std::cout << "Data atual: " << user->getStartDate() << "\n";
    std::cout << "Nova Data (DD/MM/AAAA || DDMMAAAA) ou deixe em branco: ";
    std::getline(std::cin, s);
    if(!s.empty()) {
        user->setStartDate(s);
        std::cout << ">> Data alterada para: " << user->getStartDate() << "\n";
    }

    storage.save(&sn);
    std::cout << "\n>> Sucesso: Perfil atualizado e salvo no banco de dados!\n";
}

void handleSearchUsers(Profile* user, SocialNetwork& sn, NetworkStorage& storage) {
    std::string name;
    std::cout << "\nBuscar por nome: ";
    std::getline(std::cin, name);

    auto results = sn.searchProfiles(name);
    if (results.empty()) {
        std::cout << ">> Nenhum usuario encontrado.\n";
        return;
    }

    Utils::showHeader("RESULTADOS DA BUSCA");
    for (size_t i = 0; i < results.size(); i++) {
        std::string status = "";
        User* uPtr = dynamic_cast<User*>(results[i]);
        if (uPtr && uPtr->isVerified()) status = " [VERIFICADO]";
        
        std::cout << i << ") " << results[i]->getName() << " (@" << results[i]->getId() << ")" << status 
                  << " (" << results[i]->getSubtitle() << ")\n";
    }

    std::cout << "\nDigite o numero para interagir (ou -1 para voltar): ";
    int choice;
    if (!(std::cin >> choice)) {
        std::cin.clear();
        std::cin.ignore(1000, '\n');
        return;
    }
    Utils::cleanBuffer();

    if (choice >= 0 && choice < (int)results.size()) {
        Profile* target = results[choice];
        
        std::cout << "\nOPCOES PARA " << target->getName() << ":\n";
        std::cout << "1. Enviar Pedido de Amizade\n";
        std::cout << "2. Ver Perfil Completo\n";
        std::cout << "0. Cancelar\n";
        std::cout << "Escolha: ";
        
        int action;
        std::cin >> action;
        Utils::cleanBuffer();

        if (action == 1) {
            if (target == user) {
                std::cout << ">> Voce nao pode adicionar a si mesmo.\n";
            } else {
                target->addContactRequest(user);
                
                // Notifica o alvo
                User* uTarget = dynamic_cast<User*>(target);
                if (uTarget) {
                    std::string msg = user->getName() + " enviou um pedido de amizade!";
                    uTarget->addNotification(new Notification(msg));
                }
                
                storage.save(&sn);
                std::cout << ">> Solicitacao enviada para " << target->getName() << "!\n";
            }
        } else if (action == 2) {
            target->print();
            std::cout << "\n[Pressione Enter para voltar]";
            getchar();
        }
    }
}

void handleVerify(Profile* user, SocialNetwork& sn, NetworkStorage& storage) {
    User* userPtr = dynamic_cast<User*>(user);
    
    if (!userPtr) {
        std::cout << ">> Erro: Apenas perfis de usuario podem solicitar verificacao.\n";
        return;
    }

    if (userPtr->isVerified()) {
        std::cout << ">> Voce ja possui o selo de verificacao!\n";
        return;
    }

    Utils::showHeader("SOLICITAR VERIFICACAO");
    std::cout << "Para obter o selo azul, informe seu email comercial/institucional.\n";
    std::cout << "Email: ";
    
    std::string email;
    std::getline(std::cin, email);

    if (email.empty() || email.find('@') == std::string::npos) {
        std::cout << ">> Erro: Email invalido.\n";
        return;
    }

    try {
        sn.verifyProfile(userPtr->getId(), email);
        
        storage.save(&sn);

        std::cout << ">> Parabens! O perfil " << userPtr->getName() << " agora esta VERIFICADO.\n";
    } catch (const std::exception& e) {
        std::cout << ">> Erro ao processar verificacao: " << e.what() << "\n";
    }
}

void handleTimeline(Profile* currentUser, SocialNetwork& sn, NetworkStorage& storage) {
    User* u = dynamic_cast<User*>(currentUser);
    if (!u) return;

    std::vector<Post*> posts = sn.getTimeline(u);
    Utils::showHeader("SUA LINHA DO TEMPO");

    if (posts.empty()) {
        std::cout << "Nada para mostrar ainda. Adicione amigos ou siga paginas!\n";
        std::cout << "[Pressione Enter para voltar]";
        getchar();
        return;
    }

    // Exibição dos Posts
    for (size_t i = 0; i < posts.size(); i++) {
        std::cout << "\n[" << i << "] ";
        posts[i]->print();
        std::cout << "Curtidas: " << posts[i]->getLikesCount() << " | ";
        std::cout << "Comentarios: " << posts[i]->getComments().size() << "\n";
        
        // Preview do último comentário (opcional, mas fica bem profissional)
        if (!posts[i]->getComments().empty()) {
            auto* lastCmt = posts[i]->getComments().back();
            std::cout << "  > Ultimo comentario: " << lastCmt->getAuthor()->getName() 
                      << ": " << lastCmt->getText() << "\n";
        }
        std::cout << "---------------------------------------\n";
    }

    // Menu de Interação
    std::cout << "\nDeseja interagir com algum post? (ID do post ou -1 para sair): ";
    int postIdx;
    if (!(std::cin >> postIdx)) {
        std::cin.clear();
        std::cin.ignore(1000, '\n');
        return;
    }
    Utils::cleanBuffer();

    if (postIdx >= 0 && postIdx < (int)posts.size()) {
        Post* selectedPost = posts[postIdx];
        
        std::cout << "1. Curtir\n2. Comentar\n3. Ver todos os comentarios\n0. Voltar\nEscolha: ";
        int action;
        std::cin >> action;
        Utils::cleanBuffer();

        if (action == 1) {
            try {
                selectedPost->addLike(currentUser);
                std::cout << ">> Voce curtiu a postagem de " << selectedPost->getOwner()->getName() << "!\n";
            } catch (const std::exception& e) {
                std::cout << "[!] " << e.what() << "\n";
            }
        } 
        else if (action == 2) {
            std::cout << "Escreva seu comentario: ";
            std::string commentTxt;
            std::getline(std::cin, commentTxt);
            if (!commentTxt.empty()) {
                selectedPost->addComment(commentTxt, currentUser);
                std::cout << ">> Comentario enviado!\n";
            }
        }
        else if (action == 3) {
            Utils::showHeader("COMENTARIOS");
            for (auto* c : selectedPost->getComments()) {
                std::cout << c->getAuthor()->getName() << ": " << c->getText() << "\n";
            }
            std::cout << "---------------------------\n";
            std::cout << "[Pressione Enter para continuar]";
            getchar();
        }

        // Salva as interações (curtida/comentário) no banco imediatamente
        storage.save(&sn);
    }
}

}

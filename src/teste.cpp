#include <iostream>
#include <limits>
#include <vector>
#include "SocialNetwork.h"
#include "NetworkStorage.h"
#include "User.h"
#include "Page.h"
#include "ImagePost.h" 
#include "Utils.h"
#include "Comment.h"

using namespace std;

void cleanBuffer() { cin.ignore(numeric_limits<streamsize>::max(), '\n'); }

// Funcao auxiliar para exibir cabecalho
void showHeader(string title) {
    cout << "\n=======================================" << endl;
    cout << "   " << title << endl;
    cout << "=======================================" << endl;
}

int main() {
    SocialNetwork sn;
    NetworkStorage storage("rede_social.db");
    
    // Carrega tudo
    try {
        cout << "Carregando sistema..." << endl;
        storage.load(&sn);
    } catch (const exception& e) {
        cerr << ">> ERRO FATAL ao carregar banco de dados: " << e.what() << endl;
        cout << "O programa sera iniciado com banco vazio." << endl;
        // Aqui o programa continua, mas sem travar
    }

    Profile* currentUser = nullptr; // Quem fez login (Dono da conta)
    Page* currentPage = nullptr;    // Pagina que esta sendo gerenciada (se houver)
    
    int option = -1;

    while (option != 0) {
        
        // --- CENARIO 1: VISITANTE (NINGUEM LOGADO) ---
        if (currentUser == nullptr) {
            showHeader("REDE SOCIAL - BEM VINDO");
            cout << "1. Fazer Login" << endl;
            cout << "2. Criar Nova Conta" << endl;
            cout << "0. Sair" << endl;
            cout << "Escolha: ";
            cin >> option; cleanBuffer();

            if (option == 1) {
                try {
                    string nome, senha;
                    cout << "Usuario: "; getline(cin, nome);
                    cout << "Senha: ";   getline(cin, senha);
                    currentUser = sn.login(nome, senha);
                    if (!currentUser) cout << ">> Erro: Login invalido!" << endl;
                } catch (const exception& e) {
                    cout << ">> Erro tecnico no login: " << e.what() << endl;
                }
            }
            else if (option == 2) {
                string nome, senha;
                cout << "Novo Usuario: "; getline(cin, nome);
                cout << "Nova Senha: ";   getline(cin, senha);
                // Cria User comum (Standard)
                sn.add(new User(nome, senha)); 
                cout << ">> Conta criada com sucesso! Faca login." << endl;
                storage.save(&sn);
            }
            else if (option == 0) {
                cout << "Saindo..." << endl;
            }
        } 
        
        // --- CENARIO 2: USUARIO LOGADO (GERENCIANDO O PROPRIO PERFIL) ---
        else if (currentPage == nullptr) {
            
            // Verifica status para exibir no menu
            string status = "";
            User* u = dynamic_cast<User*>(currentUser);
            if (u && u->isVerified()) status = " [VERIFICADO]";

            showHeader("DASHBOARD: " + currentUser->getName() + status);
            cout << "1. Ver Meu Perfil (Dados e Posts)" << endl;
            cout << "2. Fazer Postagem" << endl;
            cout << "3. Editar Perfil (Bio, Foto, Info)" << endl;
            cout << "4. Buscar Pessoas / Adicionar Amigo" << endl;
            cout << "5. CRIAR/GERENCIAR PAGINA" << endl; 
            cout << "6. Solicitar Verificacao (Selo Azul)" << endl;
            cout << "7. Ver TimeLine" << endl; // Nova Opcao
            cout << "9. Logout" << endl;
            cout << "0. Sair e Salvar" << endl;
            cout << "Escolha: ";
            cin >> option; cleanBuffer();

            switch (option) {
                case 1: 
                    currentUser->print(); 
                    break;
                case 2: {
                    cout << "Digite o texto: ";
                    string txt; getline(cin, txt);
                    // Aqui voce pode perguntar se quer imagem
                    cout << "Eh uma imagem? (s/n): ";
                    char resp; cin >> resp; cleanBuffer();
                    
                    if (resp == 's' || resp == 'S') {
                         cout << "Caminho do arquivo (ex: foto.png): ";
                         string path; getline(cin, path);
                         currentUser->addPost(new ImagePost(txt, currentUser, path));
                    } else {
                         currentUser->addPost(new Post(txt, currentUser));
                    }
                    cout << ">> Publicado!" << endl;
                    storage.save(&sn);
                    break;
                }
                case 3: {
                    showHeader("EDITAR PERFIL");
                    string s;

                    cout << "Nova Bio: "; getline(cin, s);
                    if(!s.empty()) currentUser->setBio(s);

                    cout << "Novo Genero/Subtitulo: "; getline(cin, s);
                    if(!s.empty()) currentUser->setSubtitle(s);

                    // Edição de Data com Throw/Catch
                    cout << "Nova Data de Nascimento (Ex: 01/01/2000 ou 01012000): "; 
                    getline(cin, s);
                    
                    if(!s.empty()) {
                        try {
                            // A função valida e já conserta a string 's' se faltar as barras
                            Utils::validateAndFixDate(s); 
                            
                            currentUser->setStartDate(s);
                            cout << ">> Sucesso: Data atualizada para " << s << endl;
                        } 
                        catch (const exception& e) {
                            // Aqui pegamos qualquer throw (invalid_argument, out_of_range, etc)
                            cout << ">> [ERRO]: " << e.what() << endl;
                        }
                    }

                    storage.save(&sn);
                    break;
                }
                case 4: { // Buscar e Adicionar
                    cout << "Search for someone: ";
                    string term; 
                    getline(cin, term);
                    
                    vector<Profile*> results = sn.searchProfiles(term);
                    
                    if (results.empty()) {
                        cout << ">> No one found with that name." << endl;
                    } else {
                        showHeader("SEARCH RESULTS");
                        for (size_t i = 0; i < results.size(); i++) {
                            // Exibimos um resumo: Nome, Cargo e Bio curta
                            cout << i << ". " << results[i]->getName();
                            if (results[i]->getRole() == "Verified User") cout << " [V]";
                            cout << " | " << results[i]->getSubtitle() << endl;
                            cout << "   Bio: " << results[i]->getBio().substr(0, 30) << "..." << endl;
                            cout << "---------------------------------------" << endl;
                        }
                        
                        cout << "Select a number to inspect (or -1 to cancel): ";
                        int idx; cin >> idx; cleanBuffer();
                        
                        if (idx >= 0 && idx < (int)results.size()) {
                            try {
                                Profile* target = results[idx];
                                if (!target) throw runtime_error("Perfil indisponivel");

                                showHeader("INSPECTING PROFILE");
                                target->print();

                                cout << "\nIs this the person you're looking for?" << endl;
                                cout << "1. Yes, add as friend" << endl;
                                cout << "2. No, back to search" << endl;
                                int conf; cin >> conf; cleanBuffer();
                                

                                if (conf == 1) {
                                    try {
                                        // Tenta adicionar
                                        currentUser->addContact(target);
                                        cout << ">> Successfully added " << target->getName() << "!" << endl;
                                        storage.save(&sn);
                                    } 
                                    catch (const std::logic_error& e) {
                                        cout << "\n[!] OPS: " << e.what() << endl;
                                        cout << "Returning to menu..." << endl;
                                    }
                                }
                            } catch (const exception& e) {
                                cout << ">> Erro ao visualizar perfil: " << e.what() << endl;
                            }
                        }
                    }
                    break;
                }
                case 5: {
                    // --- LOGICA DE PAGINAS (REFATORADA) ---
                    // 1. Garante que é um User (não uma Pagina tentando criar Pagina)
                    User* userLogado = dynamic_cast<User*>(currentUser);
                    
                    if (userLogado == nullptr) {
                        cout << ">> Erro de sistema." << endl;
                    } 
                    // 2. Verifica o booleano isVerified
                    else if (!userLogado->isVerified()) {
                        cout << ">> Apenas Usuarios Verificados podem criar paginas!" << endl;
                        cout << ">> Utilize a opcao 6 para se verificar." << endl;
                    } 
                    else {
                        cout << "1. Criar Nova Pagina" << endl;
                        cout << "2. Acessar Minha Pagina Existente" << endl;
                        int pgOpt; cin >> pgOpt; cleanBuffer();
                        
                        if (pgOpt == 1) {
                            string pgName;
                            cout << "Nome da Pagina/Empresa: "; getline(cin, pgName);
                            // Cria passando 'userLogado' como owner
                            Page* novaPagina = new Page(pgName, userLogado, "1234"); 
                            sn.add(novaPagina);
                            
                            currentPage = novaPagina; // Troca contexto
                            cout << ">> Pagina Criada! Voce agora esta gerenciando: " << pgName << endl;
                            storage.save(&sn);
                        }
                        else if (pgOpt == 2) {
                            bool found = false;
                            const auto& perfis = sn.getProfiles();
                            for (const auto& p : perfis) {
                                Page* pag = dynamic_cast<Page*>(p.get());
                                // Verifica se a pagina existe e se o ID do dono bate
                                if (pag && pag->getOwner() && pag->getOwner()->getId() == userLogado->getId()) {
                                    currentPage = pag; // Troca contexto
                                    found = true;
                                    cout << ">> Gerenciando pagina: " << pag->getName() << endl;
                                    break;
                                }
                            }
                            if (!found) cout << ">> Voce nao tem nenhuma pagina." << endl;
                        }
                    }
                    break;
                }
                case 6: {
                    // --- TORNAR-SE VERIFICADO ---
                    User* userLogado = dynamic_cast<User*>(currentUser);
                    if (userLogado->isVerified()) {
                        cout << ">> Voce ja eh verificado!" << endl;
                    } else {
                        cout << "Email comercial para validacao: ";
                        string email; getline(cin, email);
                        
                        // Chama o metodo da SocialNetwork
                        sn.verifyProfile(userLogado->getId(), email);
                        // Nao precisa recarregar ponteiro, pois foi so uma mudanca de bool
                        storage.save(&sn);
                    }
                    break;
                }

                case 7: { 
                    vector<Post*> feed = sn.getTimeline(dynamic_cast<User*>(currentUser));
                    
                    showHeader("MEU FEED DE NOTICIAS");
                    if (feed.empty()) {
                        cout << "Nada para mostrar. Adicione amigos para ver postagens!" << endl;
                    } else {
                        for (size_t i = 0; i < feed.size(); i++) {
                            cout << "\n[" << i << "] ";
                            feed[i]->print(); // Mostra o post
                            cout << "Curtidas: " << feed[i]->getLikesCount() << endl;
                            
                            // Mostrar últimos 2 comentários como "preview"
                            auto& cmts = feed[i]->getComments();
                            if(!cmts.empty()) {
                                cout << "  Comentarios (" << cmts.size() << "):" << endl;
                                cout << "  > " << cmts.back()->getAuthor()->getName() << ": " << cmts.back()->getText() << endl;
                            }
                            cout << "---------------------------------------" << endl;
                        }

                        cout << "\nDeseja interagir com algum post? (ID do post ou -1 para sair): ";
                        int postIdx; cin >> postIdx; cleanBuffer();

                        if (postIdx >= 0 && postIdx < feed.size()) {
                            cout << "1. Curtir | 2. Comentar | 3. Ver todos os comentarios: ";
                            int action; cin >> action; cleanBuffer();

                            if (action == 1) {
                                try {
                                    feed[postIdx]->addLike(currentUser);
                                    cout << "Voce curtiu isso!" << endl;
                                } catch (exception& e) { cout << e.what() << endl; }
                            } 
                            else if (action == 2) {
                                cout << "Escreva seu comentario: ";
                                string cText; getline(cin, cText);
                                feed[postIdx]->addComment(cText, currentUser);
                                cout << "Comentario enviado!" << endl;
                            }
                            storage.save(&sn); // Salva as interações no banco
                        }
                    }
                    break;
                }
                case 9: currentUser = nullptr; break;
                case 0: storage.save(&sn); return 0;
            }
        }

        // --- CENARIO 3: GERENCIANDO UMA PAGINA ---
        else {
            showHeader("MODO PAGINA: " + currentPage->getName());
            cout << "1. Ver Perfil da Pagina" << endl;
            cout << "2. Postar como Pagina" << endl;
            cout << "3. Editar Dados da Empresa" << endl;
            cout << "4. VOLTAR PARA MEU PERFIL PESSOAL" << endl; 
            cout << "Escolha: ";
            cin >> option; cleanBuffer();

            switch (option) {
                case 1: currentPage->print(); break;
                case 2: {
                    cout << "Post da Pagina: ";
                    string txt; getline(cin, txt);
                    currentPage->addPost(new Post(txt, currentPage));
                    cout << ">> Publicado na Pagina!" << endl;
                    storage.save(&sn);
                    break;
                }
                case 3: {
                    cout << "Nova Categoria: "; string s; getline(cin, s);
                    if(!s.empty()) currentPage->setSubtitle(s);
                    cout << "Nova Data Fundacao: "; getline(cin, s);
                    if(!s.empty()) currentPage->setStartDate(s);
                    cout << ">> Dados atualizados." << endl;
                    storage.save(&sn);
                    break;
                }
                case 4:
                    currentPage = nullptr; // Volta para o loop do Usuario
                    cout << ">> Voltando para perfil pessoal..." << endl;
                    break;
                default: cout << "Opcao invalida" << endl;
            }
        }
    }

    return 0;
}
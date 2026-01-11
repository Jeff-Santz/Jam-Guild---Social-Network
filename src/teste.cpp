#include <iostream>
#include <limits>
#include <vector>
#include "SocialNetwork.h"
#include "NetworkStorage.h"
#include "User.h"
#include "VerifiedUser.h"
#include "Page.h"
#include "ImagePost.h" // Nao esqueca de incluir se ja criou

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
    cout << "Carregando sistema..." << endl;
    storage.load(&sn);

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
                string nome, senha;
                cout << "Usuario: "; getline(cin, nome);
                cout << "Senha: ";   getline(cin, senha);
                currentUser = sn.login(nome, senha);
                if (!currentUser) cout << ">> Erro: Login invalido!" << endl;
                else cout << ">> Bem-vindo de volta, " << currentUser->getName() << "!" << endl;
            }
            else if (option == 2) {
                string nome, senha;
                cout << "Novo Usuario: "; getline(cin, nome);
                cout << "Nova Senha: ";   getline(cin, senha);
                // Cria com dados padrao (subtitulo e data serao padrao)
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
            showHeader("DASHBOARD: " + currentUser->getName());
            cout << "1. Ver Meu Perfil (Dados e Posts)" << endl;
            cout << "2. Fazer Postagem" << endl;
            cout << "3. Editar Perfil (Bio, Foto, Info)" << endl;
            cout << "4. Buscar Pessoas / Adicionar Amigo" << endl;
            cout << "5. CRIAR/GERENCIAR PAGINA" << endl; // <--- NOVA OPCAO
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
                    // Aqui voce pode por a logica de imagem/texto se quiser
                    currentUser->addPost(new Post(txt, currentUser));
                    cout << ">> Publicado!" << endl;
                    storage.save(&sn);
                    break;
                }
                case 3: {
                    // Menu rapido de edicao
                    cout << "Novo Subtitulo (Genero): "; string s; getline(cin, s);
                    if(!s.empty()) currentUser->setSubtitle(s);
                    cout << "Nova Bio: "; getline(cin, s);
                    if(!s.empty()) currentUser->setBio(s);
                    storage.save(&sn);
                    break;
                }
                case 4:
                    cout << ">> Recurso de busca em desenvolvimento..." << endl;
                    break;
                case 5: {
                    // --- LOGICA DE PAGINAS ---
                    // Verifica se ele é um VerifiedUser (só verificados podem ter paginas?) 
                    // Ou qualquer um pode? Vamos deixar qualquer um criar por enquanto, 
                    // mas precisamos de um VerifiedUser para ser "Dono".
                    // Se o currentUser for apenas User, nao pode ser owner de Page na nossa logica antiga.
                    // Vamos adaptar: Tentar cast para VerifiedUser.
                    
                    VerifiedUser* vUser = dynamic_cast<VerifiedUser*>(currentUser);
                    
                    if (vUser == nullptr) {
                        cout << ">> Apenas Usuarios Verificados podem criar paginas! (Regra de Negocio)" << endl;
                        // Opcional: oferecer upgrade de conta
                    } else {
                        cout << "1. Criar Nova Pagina" << endl;
                        cout << "2. Acessar Minha Pagina Existente" << endl;
                        int pgOpt; cin >> pgOpt; cleanBuffer();
                        
                        if (pgOpt == 1) {
                            string pgName;
                            cout << "Nome da Pagina/Empresa: "; getline(cin, pgName);
                            Page* novaPagina = new Page(pgName, vUser, "1234"); // Senha padrao interna
                            sn.add(novaPagina);
                            currentPage = novaPagina; // <--- TROCA O CONTEXTO
                            cout << ">> Pagina Criada! Voce agora esta gerenciando: " << pgName << endl;
                            storage.save(&sn);
                        }
                        else if (pgOpt == 2) {
                            // Procura pagina que tem este dono
                            // (Funcao ineficiente, mas funciona pra teste)
                            bool found = false;
                            const auto& perfis = sn.getProfiles();
                            for (const auto& p : perfis) {
                                Page* pag = dynamic_cast<Page*>(p.get());
                                if (pag && pag->getOwner() && pag->getOwner()->getId() == vUser->getId()) {
                                    currentPage = pag; // <--- TROCA O CONTEXTO
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
                case 9: currentUser = nullptr; break;
                case 0: storage.save(&sn); return 0;
            }
        }

        // --- CENARIO 3: GERENCIANDO UMA PAGINA ---
        else {
            showHeader("MODO PAGINA: " + currentPage->getName());
            cout << "1. Ver Perfil da Pagina" << endl;
            cout << "2. Postar como Pagina" << endl;
            cout << "3. Editar Dados da Empresa (Categoria/Fundacao)" << endl;
            cout << "4. VOLTAR PARA MEU PERFIL PESSOAL" << endl; // <--- VOLTAR
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
                    if(!s.empty()) currentPage->setSubtitle(s); // Usa o setSubtitle genericamente
                    cout << "Nova Data Fundacao: "; getline(cin, s);
                    if(!s.empty()) currentPage->setStartDate(s);
                    cout << ">> Dados da empresa atualizados." << endl;
                    storage.save(&sn);
                    break;
                }
                case 4:
                    currentPage = nullptr; // <--- AQUI A MAGICA: Volta a ser nullptr, cai no menu anterior
                    cout << ">> Voltando para perfil pessoal..." << endl;
                    break;
                default: cout << "Opcao invalida" << endl;
            }
        }
    }

    return 0;
}
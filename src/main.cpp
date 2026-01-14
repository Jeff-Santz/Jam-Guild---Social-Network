#include <iostream>
#include "Core/Database.h"
#include "Core/Translation.h"
#include "Auth/User.h"
#include "Content/Post.h" // <--- Importante!

int main() {
    auto* tr = Core::Translation::getInstance();
    auto* db = Core::Database::getInstance();
    
    // ---------------------------------------------------------
    // 1. INICIALIZAÇÃO (TABELAS)
    // ---------------------------------------------------------
    
    // Tabela Users
    std::string sqlUser = 
        "CREATE TABLE IF NOT EXISTS users ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "username TEXT NOT NULL UNIQUE, "
        "email TEXT UNIQUE, "
        "password_hash TEXT NOT NULL, "
        "bio TEXT, "
        "language TEXT DEFAULT 'pt_BR', "
        "birth_date TEXT, "
        "creation_date TEXT, "
        "is_active INTEGER DEFAULT 1"
        ");";
    db->execute(sqlUser);

    // Tabela Friendships
    std::string sqlFriends = 
        "CREATE TABLE IF NOT EXISTS friendships ("
        "user_id_1 INTEGER, "
        "user_id_2 INTEGER, "
        "since_date TEXT, "
        "PRIMARY KEY (user_id_1, user_id_2), "
        "FOREIGN KEY(user_id_1) REFERENCES users(id), "
        "FOREIGN KEY(user_id_2) REFERENCES users(id)"
        ");";
    db->execute(sqlFriends);

    // Tabela Posts (A NOVIDADE)
    std::string sqlPosts = 
        "CREATE TABLE IF NOT EXISTS posts ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "author_id INTEGER NOT NULL, "
        "content TEXT NOT NULL, "
        "creation_date TEXT, "
        "FOREIGN KEY(author_id) REFERENCES users(id)"
        ");";
    
    if(db->execute(sqlPosts)) {
        std::cout << tr->get("TABLE_CREATED") << " (Posts)" << std::endl;
    }

    // ---------------------------------------------------------
    // 2. TESTE DE CADASTRO
    // ---------------------------------------------------------
    std::cout << "\n>> [TESTE 1] Verificando usuario principal..." << std::endl;

    Auth::User tempUser;
    if (!Auth::User::findByEmail("jeff@exemplo.com", tempUser)) {
        Auth::User novoUser;
        novoUser.setUsername("JeffMaster");
        novoUser.setEmail("jeff@exemplo.com");
        novoUser.setPassword("senha123"); // Criptografa
        novoUser.setBirthDate("1999-12-31"); 
        
        if (novoUser.save()) {
            std::cout << ">> Criado com sucesso! Nascido em: " << novoUser.getBirthDate() << std::endl;
        }
    } else {
        std::cout << ">> Usuario ja existe. Nascido em: " << tempUser.getBirthDate() << std::endl;
    }

    // ---------------------------------------------------------
    // 3. TESTE DE LOGIN
    // ---------------------------------------------------------
    std::cout << "\n>> [TESTE 2] Tentando Logar..." << std::endl;
    Auth::User usuarioLogado;
    
    if (Auth::User::findByEmail("jeff@exemplo.com", usuarioLogado)) {
        std::cout << ">> Usuario encontrado: " << usuarioLogado.getUsername() << " (ID: " << usuarioLogado.getId() << ")" << std::endl;
        
        if (usuarioLogado.checkPassword("senha123")) {
            std::cout << ">> LOGIN APROVADO!" << std::endl;
        } else {
            std::cout << ">> SENHA INCORRETA!" << std::endl;
            return 1;
        }
    }

    // ---------------------------------------------------------
    // 4. TESTE SOCIAL
    // ---------------------------------------------------------
    std::cout << "\n>> [TESTE 3] Sistema Social..." << std::endl;

    Auth::User amiga;
    if (!Auth::User::findByEmail("lapis@gem.com", amiga)) {
        amiga.setUsername("LapisLazuli");
        amiga.setEmail("lapis@gem.com");
        amiga.setPassword("ocean123");
        amiga.save();
        std::cout << ">> Usuario 'LapisLazuli' criado." << std::endl;
    }

    if (usuarioLogado.addFriend(&amiga)) {
        std::cout << ">> [SUCESSO] " << usuarioLogado.getUsername() << " agora segue " << amiga.getUsername() << std::endl;
    } else {
        std::cout << ">> [INFO] Amizade ja registrada." << std::endl;
    }

    // ---------------------------------------------------------
    // 5. TESTE DE POSTS (O QUE FALTOU)
    // ---------------------------------------------------------
    std::cout << "\n>> [TESTE 4] Criando Posts..." << std::endl;

    // Jeff cria um post
    Content::Post post1;
    post1.setAuthorId(usuarioLogado.getId());
    post1.setContent("Ola mundo! Minha Engine agora tem Posts e Criptografia!");
    
    if (post1.save()) {
        std::cout << ">> Post salvo no DB com ID: " << post1.getId() << std::endl;
    }

    // Ler a timeline
    std::cout << ">> Timeline de " << usuarioLogado.getUsername() << ":" << std::endl;
    std::vector<Content::Post> timeline = Content::Post::getPostsByUserId(usuarioLogado.getId());

    if (timeline.empty()) {
        std::cout << "   (Nenhum post encontrado)" << std::endl;
    }

    for (const auto& p : timeline) {
        std::cout << "   [" << p.getCreationDate() << "] " << p.getContent() << std::endl;
    }

    return 0;
}
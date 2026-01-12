#define CROW_MAIN
#define ASIO_STANDALONE 
#include "crow_all.h"
#include "SocialNetwork.h"
#include "NetworkStorage.h"
#include "User.h"
#include <iostream>

int main() {
    // 1. INICIALIZAÇÃO DO MOTOR (Seu código antigo)
    std::cout << ">> Inicializando o motor da Rede Social...\n";
    SocialNetwork sn;
    NetworkStorage storage("rede_social.db");
    
    try {
        storage.load(&sn);
        std::cout << ">> Banco de dados carregado com sucesso!\n";
    } catch (...) {
        std::cout << ">> Aviso: Iniciando banco novo.\n";
    }

    // 2. CONFIGURAÇÃO DO CROW (O Servidor Web)
    crow::SimpleApp app;

    // --- ROTA DE TESTE ---
    // Acesse no navegador: http://localhost:8080/api/status
    CROW_ROUTE(app, "/api/status")
    ([&sn](){
        crow::json::wvalue x;
        x["status"] = "Online";
        x["server"] = "Jam Guild C++ Backend";
        x["profiles_count"] = sn.getProfilesAmount();
        return x;
    });

    // --- ROTA PARA BUSCAR PERFIL ---
    // Ex: http://localhost:8080/api/profile/1
    CROW_ROUTE(app, "/api/profile/<int>")
    ([&sn](int id){
        try {
            Profile* p = sn.getProfile(id); // Sua função já lança erro se não achar
            
            crow::json::wvalue json;
            json["id"] = p->getId();
            json["name"] = p->getName();
            json["bio"] = p->getBio();
            json["type"] = p->getRole();
            
            // Se for usuário, mostra se é verificado
            User* u = dynamic_cast<User*>(p);
            if (u) {
                json["verified"] = u->isVerified();
            }

            return crow::response(json);
        } catch (...) {
            return crow::response(404, "{\"error\": \"User not found\"}");
        }
    });

    // Rota Raiz (A página inicial da sua API)
    CROW_ROUTE(app, "/")
    ([](){
        return "<h1>Jam Guild API</h1><p>O backend está rodando! Use os endpoints /api/ para acessar os dados.</p>";
    });

    // Rota para ver a Timeline de um usuário
    CROW_ROUTE(app, "/api/timeline/<int>")
    ([&sn](int userId){
        try {
            Profile* p = sn.getProfile(userId);
            User* u = dynamic_cast<User*>(p);
            
            if (!u) return crow::response(400, "Apenas usuarios tem timeline");

            std::vector<Post*> timeline = sn.getTimeline(u);
            crow::json::wvalue x;
            
            int i = 0;
            for (auto* post : timeline) {
                x[i]["author"] = post->getOwner()->getName();
                x[i]["text"] = post->getText();
                x[i]["date"] = post->getFormattedDate();
                x[i]["type"] = post->getType();
                i++;
            }
            return crow::response(x);
        } catch (...) {
            return crow::response(404, "Usuario nao encontrado");
        }
    });

    // 3. INICIAR O SERVIDOR
    std::cout << ">> Servidor rodando em http://127.0.0.1:8085\n";
    app.bindaddr("0.0.0.0").port(8085).multithreaded().run();
    
    return 0;
}
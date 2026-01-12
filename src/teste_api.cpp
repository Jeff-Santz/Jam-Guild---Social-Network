#include <iostream>
#include <cassert> // Biblioteca para testes automáticos
#include "../include/SocialNetwork.h"
#include "../include/User.h"
#include "../include/Utils.h"

// Truque para imprimir colorido no terminal
#define GREEN "\033[32m"
#define RED "\033[31m"
#define RESET "\033[0m"

void runTests() {
    std::cout << ">>> INICIANDO TESTES DA API (ENGINE) <<<\n\n";

    // 1. Setup
    SocialNetwork sn;
    User* u1 = new User("Jeff", "senha123");
    u1->setId(1); // Forçamos um ID para teste
    sn.add(u1);

    // --- TESTE 1: verifyProfile ---
    std::cout << "[TESTE 1] verifyProfile return bool... ";
    
    // Cenário A: Sucesso
    bool res1 = sn.verifyProfile(1, "jeff@usp.br");
    if (res1 == true && u1->isVerified() == true) {
        std::cout << GREEN << "PASS (Sucesso validado)" << RESET << "\n";
    } else {
        std::cout << RED << "FAIL" << RESET << "\n";
    }

    // Cenário B: Falha (ID Inexistente)
    bool res2 = sn.verifyProfile(999, "fake@email.com");
    if (res2 == false) {
        std::cout << GREEN << "PASS (Falha validada)" << RESET << "\n";
    } else {
        std::cout << RED << "FAIL (Deveria retornar false)" << RESET << "\n";
    }

    // --- TESTE 2: getSnapshot (Antigo print) ---
    std::cout << "[TESTE 2] getSnapshot return string... ";
    std::string snap = sn.getSnapshot();
    
    // Verifica se a string não está vazia e contém o nome do usuário
    if (!snap.empty() && snap.find("Jeff") != std::string::npos) {
        std::cout << GREEN << "PASS" << RESET << "\n";
        // Opcional: Descomente para ver o resultado real
        // std::cout << "   Output: \n" << snap << "\n";
    } else {
        std::cout << RED << "FAIL" << RESET << "\n";
    }

    // --- TESTE 3: getStats (Antigo printStatistics) ---
    std::cout << "[TESTE 3] getStats return string... ";
    std::string stats = sn.getStats();
    
    if (stats.find("Total: 1") != std::string::npos) {
        std::cout << GREEN << "PASS" << RESET << "\n";
    } else {
        std::cout << RED << "FAIL" << RESET << "\n";
    }

    std::cout << "\n>>> TODOS OS TESTES FINALIZADOS <<<\n";
}

int main() {
    runTests();
    return 0;
}
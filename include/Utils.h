#ifndef UTILS_H
#define UTILS_H

#include <string>

namespace Utils {
    //Criptografia simples de senha
    std::string hashPassword(const std::string& password);
    // Valida e corrige data no formato DD/MM/AAAA
    std::string validateAndFixDate(std::string date);

    void cleanBuffer();
    void showHeader(const std::string& title);

    std::string getCurrentDate();
}

#endif
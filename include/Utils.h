#ifndef UTILS_H
#define UTILS_H

#include <string>

namespace Utils {
    //Criptografia simples de senha
    std::string hashPassword(const std::string& password);
    // Valida e corrige data no formato DD/MM/AAAA
    bool validateAndFixDate(std::string& date);
}

#endif
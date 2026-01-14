#include <iostream>
#include <string>
#include <iomanip>
#include <sstream>

std::string hashPassword(const std::string& password) {
    unsigned long hash = 5381;
    for (char c : password) {
        hash = ((hash << 5) + hash) + c;
    }
    std::stringstream ss;
    ss << std::hex << std::setw(16) << std::setfill('0') << hash;
    return ss.str();
}

int main() {
    std::string pass;
    std::cout << "Digite a nova senha para gerar o hash: ";
    std::cin >> pass;
    std::cout << "O Hash para colocar no SQLite eh: " << hashPassword(pass) << std::endl;
    return 0;
}
#ifndef LOCATION_H
#define LOCATION_H

#include <string>
#include <vector>
#include <map>

namespace Core {
    class Location {
    private:
        // Função auxiliar para ignorar maiúsculas/minúsculas
        static std::string normalize(const std::string& str);

    public:
        // Função que popula o banco na primeira vez
        static void seed();

        // Funções que agora consultam o Banco
        static std::vector<std::string> getStates(bool isEnglish = false);
        static std::vector<std::string> getCities(const std::string& state);
        static bool isValid(const std::string& city, const std::string& state);
    };
}

#endif
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
        static std::vector<std::string> getStates();
        static std::vector<std::string> getCities(const std::string& state);

        // Valida se a combinação existe (Case Insensitive!)
        static bool isValid(const std::string& city, const std::string& state);
    };
}

#endif
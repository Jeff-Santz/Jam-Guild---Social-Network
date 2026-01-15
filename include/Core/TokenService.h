#pragma once
#include <string>

namespace Core {
    class TokenService {
    private:
        static std::string getJwtSecret();
        
    public:
        static std::string createToken(int userId);
        static int verifyToken(const std::string& token);
    };
}
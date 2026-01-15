#include "Core/TokenService.h"
#include <jwt-cpp/jwt.h>
#include <chrono>
#include <cstdlib> // Para std::getenv

namespace Core {

    std::string TokenService::getJwtSecret() {
        const char* env_s = std::getenv("JWT_SECRET");
        return env_s ? std::string(env_s) : "Pr3gn4_Y0n3_J3ff1nh0_!_FALLBACK_SEGURANCA_2026";
    }

    std::string TokenService::createToken(int userId) {
        std::string secret = getJwtSecret();
        return jwt::create()
            .set_issuer("SocialEngine")
            .set_type("JWS")
            .set_payload_claim("user_id", jwt::claim(std::to_string(userId)))
            .set_expires_at(std::chrono::system_clock::now() + std::chrono::hours{24})
            .sign(jwt::algorithm::hs256{secret});
    }

    int TokenService::verifyToken(const std::string& token) {
        try {
            std::string secret = getJwtSecret();
            
            auto decoded = jwt::decode(token);
            auto verifier = jwt::verify()
                .allow_algorithm(jwt::algorithm::hs256{secret})
                .with_issuer("SocialEngine");

            verifier.verify(decoded);
            
            return std::stoi(decoded.get_payload_claim("user_id").as_string());
        } catch (...) {
            // Se o token for inválido, alterado ou expirado
            return -1; 
        }
    }
}
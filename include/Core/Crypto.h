#ifndef CRYPTO_H
#define CRYPTO_H

#include <string>

namespace Core {

    class Crypto {
    public:
        static std::string sha256(const std::string input);
    };

}

#endif
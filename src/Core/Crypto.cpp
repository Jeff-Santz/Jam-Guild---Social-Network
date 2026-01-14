#include "Core/Crypto.h"
#include <vector>
#include <sstream>
#include <iomanip>
#include <cstring>
#include <cstdint>

namespace Core {

    // Algorith SHA-256 constants
    const unsigned int k[64] = {
        0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
        0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
        0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
        0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
        0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
        0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
        0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
        0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
    };

    // Funções auxiliares de rotação de bits (Macro-like)
    inline unsigned int right_rot(unsigned int value, unsigned int count) {
        return (value >> count) | (value << (32 - count));
    }

    std::string Crypto::sha256(const std::string input) {
        unsigned int h[8] = {
            0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
            0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
        };

        std::vector<unsigned char> msg(input.begin(), input.end());
        uint64_t original_len_bits = msg.size() * 8;
        
        msg.push_back(0x80); 
        while ((msg.size() * 8) % 512 != 448) {
            msg.push_back(0x00);
        }

        for (int i = 7; i >= 0; --i) {
            msg.push_back((original_len_bits >> (i * 8)) & 0xff);
        }

        for (size_t i = 0; i < msg.size(); i += 64) {
            unsigned int w[64];
            for (int j = 0; j < 16; ++j) {
                w[j] = (msg[i + j * 4] << 24) | (msg[i + j * 4 + 1] << 16) | 
                       (msg[i + j * 4 + 2] << 8) | (msg[i + j * 4 + 3]);
            }
            for (int j = 16; j < 64; ++j) {
                unsigned int s0 = right_rot(w[j - 15], 7) ^ right_rot(w[j - 15], 18) ^ (w[j - 15] >> 3);
                unsigned int s1 = right_rot(w[j - 2], 17) ^ right_rot(w[j - 2], 19) ^ (w[j - 2] >> 10);
                w[j] = w[j - 16] + s0 + w[j - 7] + s1;
            }

            unsigned int a = h[0], b = h[1], c = h[2], d = h[3];
            unsigned int e = h[4], f = h[5], g = h[6], h_val = h[7]; // Renomeado h -> h_val para evitar conflito

            for (int j = 0; j < 64; ++j) {
                unsigned int s1 = right_rot(e, 6) ^ right_rot(e, 11) ^ right_rot(e, 25);
                unsigned int ch = (e & f) ^ ((~e) & g);
                unsigned int temp1 = h_val + s1 + ch + k[j] + w[j];
                unsigned int s0 = right_rot(a, 2) ^ right_rot(a, 13) ^ right_rot(a, 22);
                unsigned int maj = (a & b) ^ (a & c) ^ (b & c);
                unsigned int temp2 = s0 + maj;

                h_val = g; g = f; f = e; e = d + temp1;
                d = c; c = b; b = a; a = temp1 + temp2;
            }

            h[0] += a; h[1] += b; h[2] += c; h[3] += d;
            h[4] += e; h[5] += f; h[6] += g; h[7] += h_val;
        }

        std::stringstream ss;
        for (int i = 0; i < 8; ++i) {
            ss << std::hex << std::setw(8) << std::setfill('0') << h[i];
        }
        return ss.str();
    }
}
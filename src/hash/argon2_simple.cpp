#include "argon2_simple.h"
#include "sha256.h"
#include <openssl/sha.h>
#include <sstream>
#include <iomanip>
#include <cstring>

std::string Argon2Simple::hash(const std::string& password, const std::string& salt,
                                int timeCost, int memoryCost, int parallelism) {
    // Alokacija memorije (memory-hard svojstvo)
    int blocks = memoryCost / 16; // 16KB blokova
    std::vector<std::vector<uint8_t>> memory(blocks, std::vector<uint8_t>(1024));
    
    // Inicijalizacija memorije sa kombinacijom password i salt
    fillMemory(memory, password, salt);
    
    // Time cost - višestruke iteracije kroz memoriju
    for (int t = 0; t < timeCost; ++t) {
        for (int i = 0; i < blocks; ++i) {
            // Pseudo-random pristup memoriji
            int prevIndex = (i == 0) ? blocks - 1 : i - 1;
            int refIndex = (memory[prevIndex][0] * 256 + memory[prevIndex][1]) % blocks;
            
            // XOR operacija sa prethodnim blokom
            for (size_t j = 0; j < memory[i].size(); ++j) {
                memory[i][j] ^= memory[refIndex][j];
            }
            
            // Dodatno heširanje
            std::string blockData(memory[i].begin(), memory[i].end());
            unsigned char hash[SHA256_DIGEST_LENGTH];
            SHA256(reinterpret_cast<const unsigned char*>(blockData.c_str()), 
                   blockData.size(), hash);
            
            // Upisivanje hash-a nazad u memoriju
            for (int k = 0; k < 32 && k < memory[i].size(); ++k) {
                memory[i][k] = hash[k];
            }
        }
    }
    
    // Finalni hash - kombinovanje svih blokova
    std::vector<uint8_t> finalBlock(32, 0);
    for (const auto& block : memory) {
        for (size_t i = 0; i < 32 && i < block.size(); ++i) {
            finalBlock[i] ^= block[i];
        }
    }
    
    // Konverzija u hex string
    std::ostringstream oss;
    for (uint8_t byte : finalBlock) {
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
    }
    return oss.str();
}

bool Argon2Simple::verify(const std::string& password, const std::string& salt,
                          const std::string& expectedHash, int timeCost, 
                          int memoryCost, int parallelism) {
    std::string computedHash = hash(password, salt, timeCost, memoryCost, parallelism);
    return computedHash == expectedHash;
}

void Argon2Simple::fillMemory(std::vector<std::vector<uint8_t>>& memory,
                               const std::string& password, const std::string& salt) {
    std::string combined = password + salt;
    
    for (size_t i = 0; i < memory.size(); ++i) {
        std::string input = combined + std::to_string(i);
        unsigned char hash[SHA256_DIGEST_LENGTH];
        SHA256(reinterpret_cast<const unsigned char*>(input.c_str()), 
               input.size(), hash);
        
        // Popunjavanje bloka memorije
        for (size_t j = 0; j < memory[i].size(); ++j) {
            memory[i][j] = hash[j % SHA256_DIGEST_LENGTH] ^ (i + j);
        }
    }
}

std::vector<uint8_t> Argon2Simple::blake2b(const std::vector<uint8_t>& input, int outLen) {
    // Pojednostavljena verzija - koristimo SHA256 umesto Blake2b
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(input.data(), input.size(), hash);
    
    std::vector<uint8_t> result(outLen);
    for (int i = 0; i < outLen; ++i) {
        result[i] = hash[i % SHA256_DIGEST_LENGTH];
    }
    return result;
}

#pragma once
#include <cstdint>
#include <string>
#include <vector>

// Pojednostavljena verzija Argon2 algoritma
// Demonstrira memory-hard funkcionalnost za otpornost na GPU napade
class Argon2Simple {
public:
    // Generisanje hash-a sa memory-hard svojstvima
    static std::string hash(
        const std::string& password,
        const std::string& salt,
        int timeCost = 3,      // Broj iteracija
        int memoryCost = 4096, // Memorija u KB
        int parallelism = 1    // Broj paralelnih niti
    );
    
    // Verifikacija lozinke
    static bool verify(
        const std::string& password,
        const std::string& salt,
        const std::string& expectedHash,
        int timeCost = 3,
        int memoryCost = 4096,
        int parallelism = 1
    );
    
private:
    static std::vector<uint8_t> blake2b(const std::vector<uint8_t>& input, int outLen);
    static void fillMemory(std::vector<std::vector<uint8_t>>& memory, 
                          const std::string& password, 
                          const std::string& salt);
};

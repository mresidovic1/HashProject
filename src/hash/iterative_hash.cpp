#include "iterative_hash.h"
#include "sha256.h"
#include "simple_hash.h"

namespace IterativeHash {

std::string iterativeSHA256(const std::string& input, int iterations) {
    std::string hash = input;
    
    for (int i = 0; i < iterations; ++i) {
        hash = CryptoHash::sha256(hash);
    }
    
    return hash;
}

std::string iterativeSHA256WithSalt(const std::string& input, 
                                    const std::string& salt,
                                    int iterations) {
    std::string combined = input + salt;
    return iterativeSHA256(combined, iterations);
}

} // namespace IterativeHash

// Backward compatibility functions
std::string iterativeHash(const std::string& input, int iterations) {
    return IterativeHash::iterativeSHA256(input, iterations);
}

std::string simpleHash(const std::string& input) {
    return SimpleHash::djb2Hash(input);
}

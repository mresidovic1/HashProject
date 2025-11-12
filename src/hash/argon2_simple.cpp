#include "argon2_simple.h"
#include "pbkdf2.h"
#include "sha256.h"
#include "../util/util.h"
#include <sstream>
#include <vector>

namespace CryptoHash {

/**
 * This is a simplified educational implementation that simulates
 * memory-hard properties. A real Argon2 implementation would use
 * the full Argon2 algorithm with Blake2b hashing.
 */

std::string argon2Simple(const std::string& password,
                         const std::string& salt,
                         int memoryCostKB,
                         int timeCost) {
    
    // Step 1: Initial hash with PBKDF2
    // We use more iterations to simulate time cost
    int iterations = timeCost * 100000;
    std::string initialHash = pbkdf2(password, salt, iterations, 32);
    
    // Step 2: Memory-hard phase simulation
    // Create a memory buffer and fill it with derived data
    // This simulates the memory-hard property of Argon2
    int memoryBlocks = memoryCostKB / 4; // Simplified: 4KB per block
    if (memoryBlocks < 1) memoryBlocks = 1;
    
    std::vector<std::string> memoryBlocks_data;
    memoryBlocks_data.reserve(memoryBlocks);
    
    std::string currentHash = initialHash;
    
    // Fill memory with hash chains
    for (int i = 0; i < memoryBlocks; ++i) {
        currentHash = sha256(currentHash + std::to_string(i));
        memoryBlocks_data.push_back(currentHash);
    }
    
    // Step 3: Mix memory blocks (simplified mixing)
    std::string finalHash = currentHash;
    for (int t = 0; t < timeCost; ++t) {
        for (int i = 0; i < memoryBlocks; ++i) {
            int idx = i % memoryBlocks_data.size();
            finalHash = sha256(finalHash + memoryBlocks_data[idx]);
        }
    }
    
    return finalHash;
}

bool verifyArgon2Simple(const std::string& password,
                        const std::string& salt,
                        const std::string& hash,
                        int memoryCostKB,
                        int timeCost) {
    std::string computed = argon2Simple(password, salt, memoryCostKB, timeCost);
    return constantTimeCompare(computed, hash);
}

std::string argon2WithSalt(const std::string& password,
                           int memoryCostKB,
                           int timeCost) {
    std::string salt = generateSalt(16);
    std::string hash = argon2Simple(password, salt, memoryCostKB, timeCost);
    
    // Format: argon2$memory$time$salt$hash
    std::stringstream ss;
    ss << "argon2$" << memoryCostKB << "$" << timeCost << "$" << salt << "$" << hash;
    return ss.str();
}

bool verifyArgon2Hash(const std::string& password, const std::string& hashString) {
    // Parse format: argon2$memory$time$salt$hash
    std::vector<std::string> parts;
    std::stringstream ss(hashString);
    std::string part;
    
    while (std::getline(ss, part, '$')) {
        parts.push_back(part);
    }
    
    if (parts.size() != 5 || parts[0] != "argon2") {
        return false;
    }
    
    int memoryCostKB = std::stoi(parts[1]);
    int timeCost = std::stoi(parts[2]);
    std::string salt = parts[3];
    std::string expectedHash = parts[4];
    
    return verifyArgon2Simple(password, salt, expectedHash, memoryCostKB, timeCost);
}

} // namespace CryptoHash

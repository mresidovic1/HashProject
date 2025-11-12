#include "pbkdf2.h"
#include "../util/util.h"
#include <openssl/evp.h>
#include <sstream>

namespace CryptoHash {

std::string pbkdf2(const std::string& password, 
                   const std::string& salt,
                   int iterations,
                   int outputLength) {
    std::vector<uint8_t> output(outputLength);
    
    // Convert salt to bytes if it's hex
    std::vector<uint8_t> saltBytes;
    if (salt.length() % 2 == 0) {
        // Try to parse as hex
        try {
            saltBytes = hexToBytes(salt);
        } catch (...) {
            saltBytes = stringToBytes(salt);
        }
    } else {
        saltBytes = stringToBytes(salt);
    }
    
    PKCS5_PBKDF2_HMAC(
        password.c_str(), password.length(),
        saltBytes.data(), saltBytes.size(),
        iterations,
        EVP_sha256(),
        outputLength,
        output.data()
    );
    
    return bytesToHex(output);
}

bool verifyPBKDF2(const std::string& password,
                  const std::string& salt,
                  const std::string& hash,
                  int iterations,
                  int outputLength) {
    std::string computed = pbkdf2(password, salt, iterations, outputLength);
    return constantTimeCompare(computed, hash);
}

std::string pbkdf2WithSalt(const std::string& password, int iterations) {
    std::string salt = generateSalt(16);
    std::string hash = pbkdf2(password, salt, iterations, 32);
    
    // Format: iterations$salt$hash
    std::stringstream ss;
    ss << iterations << "$" << salt << "$" << hash;
    return ss.str();
}

bool verifyPBKDF2Hash(const std::string& password, const std::string& hashString) {
    // Parse format: iterations$salt$hash
    size_t firstDollar = hashString.find('$');
    size_t secondDollar = hashString.find('$', firstDollar + 1);
    
    if (firstDollar == std::string::npos || secondDollar == std::string::npos) {
        return false;
    }
    
    int iterations = std::stoi(hashString.substr(0, firstDollar));
    std::string salt = hashString.substr(firstDollar + 1, secondDollar - firstDollar - 1);
    std::string expectedHash = hashString.substr(secondDollar + 1);
    
    int outputLength = expectedHash.length() / 2; // Hex to bytes
    
    return verifyPBKDF2(password, salt, expectedHash, iterations, outputLength);
}

} // namespace CryptoHash

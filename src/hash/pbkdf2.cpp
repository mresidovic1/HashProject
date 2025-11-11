#include "pbkdf2.h"
#include <openssl/evp.h>
#include <openssl/hmac.h>
#include <cstring>
#include <sstream>
#include <iomanip>

std::string PBKDF2::derive(const std::string& password, const std::string& salt, 
                           int iterations, int keyLength) {
    std::vector<uint8_t> derivedKey(keyLength);
    
    PKCS5_PBKDF2_HMAC(
        password.c_str(), password.length(),
        reinterpret_cast<const unsigned char*>(salt.c_str()), salt.length(),
        iterations,
        EVP_sha256(),
        keyLength,
        derivedKey.data()
    );
    
    // Konvertovanje u hex string
    std::ostringstream oss;
    for (uint8_t byte : derivedKey) {
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
    }
    return oss.str();
}

bool PBKDF2::verify(const std::string& password, const std::string& salt,
                    const std::string& expectedHash, int iterations) {
    std::string computedHash = derive(password, salt, iterations, expectedHash.length() / 2);
    return computedHash == expectedHash;
}

std::vector<uint8_t> PBKDF2::hmacSHA256(const std::vector<uint8_t>& key,
                                        const std::vector<uint8_t>& data) {
    std::vector<uint8_t> result(32);
    unsigned int len;
    
    HMAC(EVP_sha256(), 
         key.data(), key.size(),
         data.data(), data.size(),
         result.data(), &len);
    
    return result;
}

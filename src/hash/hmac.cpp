#include "hmac.h"
#include "../util/util.h"
#include <openssl/hmac.h>
#include <openssl/evp.h>

namespace CryptoHash {

std::string hmacSHA256(const std::string& message, const std::string& key) {
    return hmacSHA256(
        reinterpret_cast<const uint8_t*>(message.c_str()), message.length(),
        reinterpret_cast<const uint8_t*>(key.c_str()), key.length()
    );
}

std::string hmacSHA256(const uint8_t* message, size_t messageLen, 
                       const uint8_t* key, size_t keyLen) {
    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int hashLen;
    
    HMAC(EVP_sha256(), key, keyLen, message, messageLen, hash, &hashLen);
    
    return bytesToHex(hash, hashLen);
}

bool verifyHMAC(const std::string& message, const std::string& key, 
                const std::string& expectedHmac) {
    std::string computedHmac = hmacSHA256(message, key);
    return constantTimeCompare(computedHmac, expectedHmac);
}

} // namespace CryptoHash

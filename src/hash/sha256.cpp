#include "sha256.h"
#include "../util/util.h"
#include <openssl/sha.h>
#include <openssl/evp.h>
#include <cstring>

namespace CryptoHash {

std::string sha256(const std::string& input) {
    return sha256(reinterpret_cast<const uint8_t*>(input.c_str()), input.length());
}

std::string sha256(const uint8_t* data, size_t length) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    
    // Use OpenSSL's EVP interface (recommended modern approach)
    EVP_MD_CTX* context = EVP_MD_CTX_new();
    EVP_DigestInit_ex(context, EVP_sha256(), nullptr);
    EVP_DigestUpdate(context, data, length);
    EVP_DigestFinal_ex(context, hash, nullptr);
    EVP_MD_CTX_free(context);
    
    return bytesToHex(hash, SHA256_DIGEST_LENGTH);
}

std::string sha256(const std::vector<uint8_t>& data) {
    return sha256(data.data(), data.size());
}

std::string sha256WithSalt(const std::string& password, const std::string& salt) {
    std::string combined = password + salt;
    return sha256(combined);
}

} // namespace CryptoHash

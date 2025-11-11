#include "hmac.h"
#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <fstream>
#include <sstream>
#include <iomanip>

std::string HMAC::generate(const std::string& key, const std::string& message) {
    unsigned char result[EVP_MAX_MD_SIZE];
    unsigned int resultLen;
    
    ::HMAC(EVP_sha256(),
           key.c_str(), key.length(),
           reinterpret_cast<const unsigned char*>(message.c_str()), message.length(),
           result, &resultLen);
    
    std::ostringstream oss;
    for (unsigned int i = 0; i < resultLen; ++i) {
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(result[i]);
    }
    return oss.str();
}

bool HMAC::verify(const std::string& key, const std::string& message,
                  const std::string& expectedHMAC) {
    std::string computedHMAC = generate(key, message);
    return constantTimeCompare(computedHMAC, expectedHMAC);
}

std::string HMAC::generateForFile(const std::string& key, const std::string& filePath) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        return "";
    }
    
    std::ostringstream contentStream;
    contentStream << file.rdbuf();
    std::string content = contentStream.str();
    
    return generate(key, content);
}

bool HMAC::constantTimeCompare(const std::string& a, const std::string& b) {
    if (a.length() != b.length()) {
        return false;
    }
    
    volatile int result = 0;
    for (size_t i = 0; i < a.length(); ++i) {
        result |= a[i] ^ b[i];
    }
    return result == 0;
}

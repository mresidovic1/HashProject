#include "iterative_hash.h"
#include "sha256.h"
#include <functional>
#include <openssl/sha.h>
#include <sstream>
#include <iomanip>

std::string iterativeHash(const std::string &input, int iterations) {
    std::string h = input;
    for (int i = 0; i < iterations; i++) {
        h = optimizedSHA256(h);
    }
    return h;
}

std::string optimizedSHA256(const std::string &data) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(data.c_str()), data.size(), hash);

    std::ostringstream oss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    }
    return oss.str();
}

std::string optimizedSimpleHash(const std::string &input) {
    std::hash<std::string> hasher;
    size_t hashValue = hasher(input);
    return std::to_string(hashValue);
}

// Uklonjena implementacija simpleHash jer je već definisana u simple_hash.cpp

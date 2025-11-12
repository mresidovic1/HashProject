#include "simple_hash.h"
#include "../util/util.h"
#include <sstream>
#include <iomanip>

namespace SimpleHash {

std::string additiveHash(const std::string& input) {
    uint64_t hash = 0;
    
    for (char c : input) {
        hash += static_cast<uint8_t>(c);
    }
    
    std::stringstream ss;
    ss << std::hex << std::setfill('0') << std::setw(16) << hash;
    return ss.str();
}

std::string djb2Hash(const std::string& input) {
    uint64_t hash = 5381;
    
    for (char c : input) {
        hash = ((hash << 5) + hash) + static_cast<uint8_t>(c); // hash * 33 + c
    }
    
    std::stringstream ss;
    ss << std::hex << std::setfill('0') << std::setw(16) << hash;
    return ss.str();
}

std::string fnv1aHash(const std::string& input) {
    // FNV-1a 64-bit
    uint64_t hash = 14695981039346656037ULL; // FNV offset basis
    const uint64_t fnv_prime = 1099511628211ULL;
    
    for (char c : input) {
        hash ^= static_cast<uint8_t>(c);
        hash *= fnv_prime;
    }
    
    std::stringstream ss;
    ss << std::hex << std::setfill('0') << std::setw(16) << hash;
    return ss.str();
}

std::string polynomialHash(const std::string& input) {
    const uint64_t p = 31; // Prime number
    const uint64_t m = 1e9 + 9; // Large prime modulus
    
    uint64_t hash = 0;
    uint64_t p_pow = 1;
    
    for (char c : input) {
        hash = (hash + (static_cast<uint8_t>(c) * p_pow) % m) % m;
        p_pow = (p_pow * p) % m;
    }
    
    std::stringstream ss;
    ss << std::hex << std::setfill('0') << std::setw(16) << hash;
    return ss.str();
}

std::string murmurHash3Simple(const std::string& input) {
    // Simplified MurmurHash3-inspired implementation
    uint64_t h = 0x1234567890ABCDEFULL; // Seed
    
    const uint64_t c1 = 0x87c37b91114253d5ULL;
    const uint64_t c2 = 0x4cf5ad432745937fULL;
    
    for (size_t i = 0; i < input.length(); ++i) {
        uint64_t k = static_cast<uint8_t>(input[i]);
        
        k *= c1;
        k = (k << 31) | (k >> (64 - 31)); // Rotate left 31
        k *= c2;
        
        h ^= k;
        h = (h << 27) | (h >> (64 - 27)); // Rotate left 27
        h = h * 5 + 0x52dce729;
    }
    
    // Finalization
    h ^= input.length();
    h ^= (h >> 33);
    h *= 0xff51afd7ed558ccdULL;
    h ^= (h >> 33);
    h *= 0xc4ceb9fe1a85ec53ULL;
    h ^= (h >> 33);
    
    std::stringstream ss;
    ss << std::hex << std::setfill('0') << std::setw(16) << h;
    return ss.str();
}

} // namespace SimpleHash

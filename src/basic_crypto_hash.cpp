#include "basic_crypto_hash.hpp"
#include <cstring>
#include <iostream>

namespace hashing {

const uint32_t BasicCryptoHash::K[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

#define ROTR(x, n) (((x) >> (n)) | ((x) << (32 - (n))))
#define CH(x, y, z) (((x) & (y)) ^ (~(x) & (z)))
#define MAJ(x, y, z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))
#define EP0(x) (ROTR(x, 2) ^ ROTR(x, 13) ^ ROTR(x, 22))
#define EP1(x) (ROTR(x, 6) ^ ROTR(x, 11) ^ ROTR(x, 25))
#define SIG0(x) (ROTR(x, 7) ^ ROTR(x, 18) ^ ((x) >> 3))
#define SIG1(x) (ROTR(x, 17) ^ ROTR(x, 19) ^ ((x) >> 10))

void BasicCryptoHash::sha256_transform(uint32_t state[8], const uint8_t block[64]) const {
    uint32_t w[64];
    uint32_t a, b, c, d, e, f, g, h, t1, t2;
    
    // Prepare message schedule
    for (int i = 0; i < 16; i++) {
        w[i] = (block[i * 4] << 24) | (block[i * 4 + 1] << 16) |
               (block[i * 4 + 2] << 8) | block[i * 4 + 3];
    }
    for (int i = 16; i < 64; i++) {
        w[i] = SIG1(w[i - 2]) + w[i - 7] + SIG0(w[i - 15]) + w[i - 16];
    }
    
    // Initialize working variables
    a = state[0]; b = state[1]; c = state[2]; d = state[3];
    e = state[4]; f = state[5]; g = state[6]; h = state[7];
    
    // Main loop
    for (int i = 0; i < 64; i++) {
        t1 = h + EP1(e) + CH(e, f, g) + K[i] + w[i];
        t2 = EP0(a) + MAJ(a, b, c);
        h = g; g = f; f = e; e = d + t1;
        d = c; c = b; b = a; a = t1 + t2;
    }
    
    // Update state
    state[0] += a; state[1] += b; state[2] += c; state[3] += d;
    state[4] += e; state[5] += f; state[6] += g; state[7] += h;
}

void BasicCryptoHash::sha256_compute(const uint8_t* data, size_t len, uint8_t hash[32]) const {
    uint32_t state[8] = {
        0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
        0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
    };
    
    uint8_t block[64];
    size_t i = 0;
    
    // Process full blocks
    while (i + 64 <= len) {
        std::memcpy(block, data + i, 64);
        sha256_transform(state, block);
        i += 64;
    }
    
    // Padding
    size_t remaining = len - i;
    std::memcpy(block, data + i, remaining);
    block[remaining] = 0x80;
    
    if (remaining >= 56) {
        std::memset(block + remaining + 1, 0, 64 - remaining - 1);
        sha256_transform(state, block);
        std::memset(block, 0, 56);
    } else {
        std::memset(block + remaining + 1, 0, 56 - remaining - 1);
    }
    
    // Append length
    uint64_t bit_len = len * 8;
    for (int j = 0; j < 8; j++) {
        block[63 - j] = static_cast<uint8_t>(bit_len >> (j * 8));
    }
    sha256_transform(state, block);
    
    // Produce output
    for (int j = 0; j < 8; j++) {
        hash[j * 4] = static_cast<uint8_t>(state[j] >> 24);
        hash[j * 4 + 1] = static_cast<uint8_t>(state[j] >> 16);
        hash[j * 4 + 2] = static_cast<uint8_t>(state[j] >> 8);
        hash[j * 4 + 3] = static_cast<uint8_t>(state[j]);
    }
}

void BasicCryptoHash::build(const std::vector<std::string>& /* keys */) {
    // No build phase needed for cryptographic hash
}

uint64_t BasicCryptoHash::hash(const std::string& key) const {
    uint8_t hash_output[32];
    sha256_compute(reinterpret_cast<const uint8_t*>(key.data()), key.size(), hash_output);
    
    // Return first 64 bits
    uint64_t result = 0;
    for (int i = 0; i < 8; i++) {
        result = (result << 8) | hash_output[i];
    }
    return result;
}

size_t BasicCryptoHash::getMemoryUsage() const {
    return sizeof(*this);
}

void BasicCryptoHash::printStats() const {
    std::cout << "  No precomputation required\n";
    std::cout << "  Pure cryptographic hash function\n";
    std::cout << "  256-bit output (using first 64 bits)\n";
}

} // namespace hashing

#ifndef BASIC_CRYPTO_HASH_HPP
#define BASIC_CRYPTO_HASH_HPP

#include "base_hash.hpp"

namespace hashing {

// SHA-256 implementation for baseline cryptographic hashing
class BasicCryptoHash : public HashModel {
private:
    static const uint32_t K[64];
    
    void sha256_transform(uint32_t state[8], const uint8_t block[64]) const;
    void sha256_compute(const uint8_t* data, size_t len, uint8_t hash[32]) const;
    
public:
    std::string getName() const override { return "Basic Cryptographic Hash (SHA-256)"; }
    void build(const std::vector<std::string>& keys) override;
    uint64_t hash(const std::string& key) const override;
    size_t getMemoryUsage() const override;
    void printStats() const override;
};

} // namespace hashing

#endif // BASIC_CRYPTO_HASH_HPP

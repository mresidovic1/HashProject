#ifndef SIPHASH_HPP
#define SIPHASH_HPP

#include "base_hash.hpp"

namespace hashing {

// SipHash-2-4: Fast keyed hash function resistant to hash-flooding DoS
class SipHash : public HashModel {
private:
    uint64_t k0, k1;  // 128-bit key
    
    void sipround(uint64_t& v0, uint64_t& v1, uint64_t& v2, uint64_t& v3) const;
    uint64_t siphash24(const uint8_t* data, size_t len) const;
    
public:
    SipHash();
    explicit SipHash(uint64_t key0, uint64_t key1);
    
    std::string getName() const override { return "SipHash-2-4 (Keyed)"; }
    void build(const std::vector<std::string>& keys) override;
    uint64_t hash(const std::string& key) const override;
    size_t getMemoryUsage() const override;
    void printStats() const override;
};

} // namespace hashing

#endif // SIPHASH_HPP

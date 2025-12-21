#ifndef HYBRID_CRYPTO_PERFECT_HPP
#define HYBRID_CRYPTO_PERFECT_HPP

#include "base_hash.hpp"
#include "bdz_mphf.hpp"
#include "siphash.hpp"

namespace hashing {

// Hybrid Model 1: Cryptographic Preprocessing + Perfect Hash Lookup
// Use SipHash to reduce keys to fingerprints, then MPHF for collision-free indexing
class HybridCryptoPerfect : public HashModel {
private:
    BDZ_MPHF perfect_hash;
    SipHash crypto_hash;
    std::vector<uint64_t> fingerprints;  // Cryptographic fingerprints
    
public:
    std::string getName() const override { 
        return "Hybrid: SipHash Preprocessing + BDZ MPHF"; 
    }
    void build(const std::vector<std::string>& keys) override;
    uint64_t hash(const std::string& key) const override;
    size_t getMemoryUsage() const override;
    void printStats() const override;
};

} // namespace hashing

#endif // HYBRID_CRYPTO_PERFECT_HPP

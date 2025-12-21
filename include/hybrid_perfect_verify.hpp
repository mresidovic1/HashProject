#ifndef HYBRID_PERFECT_VERIFY_HPP
#define HYBRID_PERFECT_VERIFY_HPP

#include "base_hash.hpp"
#include "cuckoo_perfect_hash.hpp"
#include "blake3_hash.hpp"

namespace hashing {

// Hybrid Model 2: Perfect Hash Indexing + Cryptographic Verification
// Use perfect hash for O(1) lookup, then BLAKE3 for collision detection
class HybridPerfectVerify : public HashModel {
private:
    CuckooPerfectHash perfect_hash;
    BLAKE3Hash crypto_verifier;
    std::vector<uint64_t> verification_codes;  // BLAKE3 checksums
    
public:
    std::string getName() const override { 
        return "Hybrid: Cuckoo Perfect Index + BLAKE3 Verification"; 
    }
    void build(const std::vector<std::string>& keys) override;
    uint64_t hash(const std::string& key) const override;
    size_t getMemoryUsage() const override;
    void printStats() const override;
};

} // namespace hashing

#endif // HYBRID_PERFECT_VERIFY_HPP

#ifndef ADAPTIVE_SECURITY_HYBRID_HPP
#define ADAPTIVE_SECURITY_HYBRID_HPP

#include "base_hash.hpp"
#include <vector>
#include <string>

namespace hashing {

// Adaptive Security Hybrid: Runtime-tunable cryptographic verification
// 
// Architecture:
// Stage 1: SipHash preprocessing (DoS resistance)
// Stage 2: BDZ MPHF (collision-free indexing)
// Stage 3: Conditional BLAKE3 verification (based on security level)
//
// Security Levels:
//   0-1: Low    (no verification)
//   2-4: Medium (1/16 sampling)
//   5-7: High   (full verification)
class AdaptiveSecurityHybrid : public HashModel {
private:
    uint64_t sip_key0, sip_key1;
    std::vector<uint8_t> g_table;
    size_t num_keys;
    size_t table_size;
    uint64_t mphf_seeds[3];
    std::vector<uint64_t> blake3_fingerprints;
    
    uint8_t security_level; // 0-7, controls verification frequency
    
    ConstructionStats construction_stats;

    uint64_t siphash_stage(const std::string& key) const;
    size_t mphf_stage(uint64_t preprocessed) const;
    uint64_t blake3_stage(const std::string& key) const;
    void compute_mphf_hashes(uint64_t preprocessed, size_t& h0, size_t& h1, size_t& h2) const;
    bool build_mphf(const std::vector<std::string>& keys);
    bool should_verify(const std::string& key) const;
    double compute_chi_square(const std::vector<std::string>& keys) const;

public:
    AdaptiveSecurityHybrid() : security_level(4) {} // Default: medium
    
    void setSecurityLevel(uint8_t level) { security_level = level & 0x07; }
    uint8_t getSecurityLevel() const { return security_level; }
    
    std::string getName() const override;
    void build(const std::vector<std::string>& keys) override;
    uint64_t hash(const std::string& key) const override;
    size_t getMemoryUsage() const override;
    void printStats() const override;
    ConstructionStats getConstructionStats() const override { return construction_stats; }
};

} // namespace hashing

#endif // ADAPTIVE_SECURITY_HYBRID_HPP

#ifndef TWO_PATH_HYBRID_HPP
#define TWO_PATH_HYBRID_HPP

#include "base_hash.hpp"
#include <vector>
#include <string>

namespace hashing {

// Two-Path Hybrid: Dual hash tables for fast/secure lanes
// 
// Architecture:
// Fast Lane:  SipHash-1-2 + Simple MPHF (no verification)
// Secure Lane: SipHash-2-4 + BDZ + BLAKE3 (full security)
// Router: Deterministic lane assignment via key hash LSB
class TwoPathHybrid : public HashModel {
private:
    // Fast lane structures
    uint64_t fast_sip_key0, fast_sip_key1;
    std::vector<uint8_t> fast_g_table;
    size_t fast_table_size;
    uint64_t fast_seeds[3];
    
    // Secure lane structures
    uint64_t secure_sip_key0, secure_sip_key1;
    std::vector<uint8_t> secure_g_table;
    size_t secure_table_size;
    uint64_t secure_seeds[3];
    std::vector<uint64_t> secure_fingerprints;
    
    size_t num_keys;

    ConstructionStats construction_stats;

    // Routing
    bool route_to_secure_lane(const std::string& key) const;
    
    // Fast lane operations
    uint64_t fast_siphash(const std::string& key) const;
    size_t fast_mphf(uint64_t preprocessed) const;
    
    // Secure lane operations
    uint64_t secure_siphash(const std::string& key) const;
    size_t secure_mphf(uint64_t preprocessed) const;
    uint64_t blake3_stage(const std::string& key) const;
    
    void compute_mphf_hashes(uint64_t preprocessed, const uint64_t seeds[3],
                            size_t table_sz, size_t& h0, size_t& h1, size_t& h2) const;
    bool build_mphf(const std::vector<std::string>& keys, std::vector<uint8_t>& g_table,
                   size_t table_sz, const uint64_t seeds[3], uint64_t sip_k0, uint64_t sip_k1);
    double compute_chi_square(const std::vector<std::string>& keys) const;

public:
    std::string getName() const override { 
        return "Two-Path Hybrid: Dual (Fast + Secure) Lanes"; 
    }
    void build(const std::vector<std::string>& keys) override;
    uint64_t hash(const std::string& key) const override;
    size_t getMemoryUsage() const override;
    void printStats() const override;
    ConstructionStats getConstructionStats() const override { return construction_stats; }
};

} // namespace hashing

#endif // TWO_PATH_HYBRID_HPP

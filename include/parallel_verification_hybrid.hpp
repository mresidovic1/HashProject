#ifndef PARALLEL_VERIFICATION_HYBRID_HPP
#define PARALLEL_VERIFICATION_HYBRID_HPP

#include "base_hash.hpp"
#include <vector>
#include <string>
#include <atomic>

namespace hashing {

// Parallel Verification Hybrid: Latency hiding through concurrent operations
// 
// Architecture:
// Parallel Track 1: SipHash → BDZ-MPHF → Fetch Index
// Parallel Track 2: BLAKE3 computation (started early)
// Join: Verify and return
class ParallelVerificationHybrid : public HashModel {
private:
    uint64_t sip_key0, sip_key1;
    std::vector<uint8_t> g_table;
    size_t num_keys;
    size_t table_size;
    uint64_t mphf_seeds[3];
    std::vector<uint64_t> blake3_fingerprints;
    
    // Prefetch metadata for cache optimization
    std::vector<uint8_t> prefetch_hints;
    
    uint64_t siphash_stage(const std::string& key) const;
    size_t mphf_stage(uint64_t preprocessed) const;
    uint64_t blake3_stage(const std::string& key) const;
    void compute_mphf_hashes(uint64_t preprocessed, size_t& h0, size_t& h1, size_t& h2) const;
    bool build_mphf(const std::vector<std::string>& keys);
    
public:
    std::string getName() const override { 
        return "Parallel Verification Hybrid: SipHash || (BDZ + BLAKE3)"; 
    }
    void build(const std::vector<std::string>& keys) override;
    uint64_t hash(const std::string& key) const override;
    size_t getMemoryUsage() const override;
    void printStats() const override;
};

} // namespace hashing

#endif // PARALLEL_VERIFICATION_HYBRID_HPP

#ifndef CACHE_PARTITIONED_HYBRID_HPP
#define CACHE_PARTITIONED_HYBRID_HPP

#include "base_hash.hpp"
#include <vector>
#include <string>

namespace hashing {

// Cache-Partitioned Security Hybrid: Hardware-aware data layout
// 
// Architecture:
// L1 Hot Path: SipHash state + compressed g-table + Bloom filter
// L2 Warm: BLAKE3 fingerprints (fetched on hit)
// Early exit on Bloom filter miss
class CachePartitionedHybrid : public HashModel {
private:
    uint64_t sip_key0, sip_key1;
    std::vector<uint8_t> g_table;
    size_t num_keys;
    size_t table_size;
    uint64_t mphf_seeds[3];
    std::vector<uint64_t> blake3_fingerprints;
    
    // Bloom filter for fast negative lookups (L1-resident)
    std::vector<uint64_t> bloom_filter;
    static constexpr size_t BLOOM_BITS_PER_KEY = 8;
    size_t bloom_size;
    uint64_t bloom_seeds[3];

    ConstructionStats construction_stats;

    uint64_t siphash_stage(const std::string& key) const;
    size_t mphf_stage(uint64_t preprocessed) const;
    uint64_t blake3_stage(const std::string& key) const;
    void compute_mphf_hashes(uint64_t preprocessed, size_t& h0, size_t& h1, size_t& h2) const;
    bool build_mphf(const std::vector<std::string>& keys);
    double compute_chi_square(const std::vector<std::string>& keys) const;

    bool bloom_check(const std::string& key) const;
    void bloom_insert(const std::string& key);

public:
    std::string getName() const override { 
        return "Cache-Partitioned Hybrid: Bloom + SipHash + BDZ(L1) + BLAKE3(L2)"; 
    }
    void build(const std::vector<std::string>& keys) override;
    uint64_t hash(const std::string& key) const override;
    size_t getMemoryUsage() const override;
    void printStats() const override;
    ConstructionStats getConstructionStats() const override { return construction_stats; }
};

} // namespace hashing

#endif // CACHE_PARTITIONED_HYBRID_HPP

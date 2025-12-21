#ifndef ULTIMATE_HYBRID_HPP
#define ULTIMATE_HYBRID_HPP

#include "base_hash.hpp"
#include <memory>

namespace hashing {

// Ultimate Hybrid Model: Multi-Stage Security-Aware Perfect Hashing
// 
// Architecture:
// Stage 1: SipHash keyed preprocessing (DoS resistance)
// Stage 2: BDZ MPHF for collision-free indexing (space-efficient)
// Stage 3: BLAKE3 cryptographic verification (security)
// Stage 4: Locality-aware cache optimization
//
// Features:
// - O(1) lookup with collision-free guarantee
// - Cryptographic security properties
// - DoS attack resistance
// - Memory-efficient (2-3 bits per key for MPHF)
// - Cache-friendly access patterns
class UltimateHybridHash : public HashModel {
private:
    // Stage 1: Keyed preprocessing
    uint64_t sip_key0, sip_key1;
    
    // Stage 2: MPHF structure (BDZ-based)
    std::vector<uint8_t> g_table;
    size_t num_keys;
    size_t table_size;
    uint64_t mphf_seeds[3];
    
    // Stage 3: Verification layer
    std::vector<uint64_t> blake3_fingerprints;
    
    // Stage 4: Cache-aware structure
    static constexpr size_t CACHE_LINE_SIZE = 64;
    struct alignas(CACHE_LINE_SIZE) CacheBlock {
        uint64_t fingerprints[8];
    };
    std::vector<CacheBlock> cache_optimized_storage;
    
    // Helper functions
    uint64_t siphash_stage(const std::string& key) const;
    size_t mphf_stage(uint64_t preprocessed) const;
    uint64_t blake3_stage(const std::string& key) const;
    bool verify_lookup(const std::string& key, size_t index) const;
    
    void compute_mphf_hashes(uint64_t preprocessed, size_t& h0, size_t& h1, size_t& h2) const;
    bool build_mphf(const std::vector<std::string>& keys);
    
public:
    std::string getName() const override { 
        return "Ultimate Hybrid: SipHash + BDZ-MPHF + BLAKE3 + Cache-Aware"; 
    }
    void build(const std::vector<std::string>& keys) override;
    uint64_t hash(const std::string& key) const override;
    size_t getMemoryUsage() const override;
    void printStats() const override;
    
    // Additional analysis methods
    double getAverageProbeLength() const;
    double getCacheEfficiency() const;
    void printSecurityAnalysis() const;
};

} // namespace hashing

#endif // ULTIMATE_HYBRID_HPP

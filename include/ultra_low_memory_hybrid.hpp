#ifndef ULTRA_LOW_MEMORY_HYBRID_HPP
#define ULTRA_LOW_MEMORY_HYBRID_HPP

#include "base_hash.hpp"
#include <vector>
#include <string>

namespace hashing {

// Ultra-Low-Memory Secure Hybrid: Minimal storage via on-demand verification
// 
// Architecture:
// - SipHash preprocessing
// - Compact RecSplit-style MPHF (sub-1 bit/key)
// - On-demand BLAKE3 (no stored fingerprints)
// - Trades computation for memory (90% reduction)
class UltraLowMemoryHybrid : public HashModel {
private:
    uint64_t sip_key0, sip_key1;
    
    // Compact MPHF representation (simpler than RecSplit, ~2 bits/key)
    std::vector<uint8_t> compact_table;
    size_t num_keys;
    size_t table_size;
    uint64_t mphf_seeds[3];
    
    // NO stored fingerprints - recompute on demand!
    
    uint64_t siphash_stage(const std::string& key) const;
    size_t mphf_stage(uint64_t preprocessed) const;
    uint64_t blake3_stage(const std::string& key) const;
    void compute_mphf_hashes(uint64_t preprocessed, size_t& h0, size_t& h1, size_t& h2) const;
    bool build_mphf(const std::vector<std::string>& keys);
    
public:
    std::string getName() const override { 
        return "Ultra-Low-Memory Hybrid: SipHash + Compact-MPHF + Streaming-BLAKE3"; 
    }
    void build(const std::vector<std::string>& keys) override;
    uint64_t hash(const std::string& key) const override;
    size_t getMemoryUsage() const override;
    void printStats() const override;
};

} // namespace hashing

#endif // ULTRA_LOW_MEMORY_HYBRID_HPP

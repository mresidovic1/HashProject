#ifndef BDZ_MPHF_HPP
#define BDZ_MPHF_HPP

#include "base_hash.hpp"
#include <vector>

namespace hashing {

// BDZ Minimal Perfect Hash Function
// Reference: Belazzougui, Botelho, Dietzfelbinger (2009)
class BDZ_MPHF : public HashModel {
private:
    std::vector<uint8_t> g_values;  // 2-bit values packed
    size_t num_keys;
    size_t table_size;
    uint64_t seeds[3];
    ConstructionStats construction_stats;

    void compute_three_hashes(const std::string& key, size_t& h0, size_t& h1, size_t& h2) const;
    bool build_graph_and_assign(const std::vector<std::string>& keys);
    uint8_t get_g_value(size_t index) const;
    void set_g_value(size_t index, uint8_t value);
    double compute_chi_square(const std::vector<std::string>& keys) const;

public:
    std::string getName() const override { return "BDZ Minimal Perfect Hash"; }
    void build(const std::vector<std::string>& keys) override;
    uint64_t hash(const std::string& key) const override;
    size_t getMemoryUsage() const override;
    void printStats() const override;
    ConstructionStats getConstructionStats() const override { return construction_stats; }
};

} // namespace hashing

#endif // BDZ_MPHF_HPP

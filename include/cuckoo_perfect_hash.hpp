#ifndef CUCKOO_PERFECT_HASH_HPP
#define CUCKOO_PERFECT_HASH_HPP

#include "base_hash.hpp"
#include <optional>

namespace hashing {

// Cuckoo-based perfect hashing with d-ary cuckoo approach
class CuckooPerfectHash : public HashModel {
private:
    static constexpr size_t NUM_TABLES = 3;
    static constexpr size_t MAX_ITERATIONS = 500;
    
    struct Slot {
        std::string key;
        bool occupied;
    };
    
    std::vector<std::vector<Slot>> tables;
    uint64_t seeds[NUM_TABLES];
    size_t num_keys;
    size_t table_size;
    
    size_t hash_to_table(const std::string& key, size_t table_idx) const;
    bool insert_with_rehash(const std::string& key);
    
public:
    std::string getName() const override { return "Cuckoo Perfect Hash"; }
    void build(const std::vector<std::string>& keys) override;
    uint64_t hash(const std::string& key) const override;
    size_t getMemoryUsage() const override;
    void printStats() const override;
};

} // namespace hashing

#endif // CUCKOO_PERFECT_HASH_HPP

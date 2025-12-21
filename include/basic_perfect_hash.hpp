#ifndef BASIC_PERFECT_HASH_HPP
#define BASIC_PERFECT_HASH_HPP

#include "base_hash.hpp"
#include <unordered_map>

namespace hashing {

// Two-level perfect hashing (Fredman, Komlós, Szemerédi)
class BasicPerfectHash : public HashModel {
private:
    struct SecondLevel {
        std::vector<std::string> table;
        uint64_t seed;
        size_t size;
    };
    
    std::vector<SecondLevel> first_level;
    size_t num_keys;
    uint64_t primary_seed;
    
    uint64_t hash_function(const std::string& key, uint64_t seed) const;
    bool build_second_level(size_t index, const std::vector<std::string>& bucket_keys);
    
public:
    std::string getName() const override { return "Basic Perfect Hash (FKS)"; }
    void build(const std::vector<std::string>& keys) override;
    uint64_t hash(const std::string& key) const override;
    size_t getMemoryUsage() const override;
    void printStats() const override;
};

} // namespace hashing

#endif // BASIC_PERFECT_HASH_HPP

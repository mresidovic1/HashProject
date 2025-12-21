#include "basic_perfect_hash.hpp"
#include <random>
#include <unordered_map>
#include <iostream>

namespace hashing {

uint64_t BasicPerfectHash::hash_function(const std::string& key, uint64_t seed) const {
    uint64_t hash = seed;
    for (char c : key) {
        hash = hash * 31 + static_cast<uint64_t>(c);
    }
    return hash;
}

void BasicPerfectHash::build(const std::vector<std::string>& keys) {
    num_keys = keys.size();
    size_t m = num_keys;  // First level size
    
    std::random_device rd;
    std::mt19937_64 gen(rd());
    primary_seed = gen();
    
    // First level: distribute keys into buckets
    std::unordered_map<size_t, std::vector<std::string>> buckets;
    for (const auto& key : keys) {
        size_t bucket = hash_function(key, primary_seed) % m;
        buckets[bucket].push_back(key);
    }
    
    // Second level: build collision-free tables
    first_level.resize(m);
    for (size_t i = 0; i < m; i++) {
        if (buckets.find(i) != buckets.end()) {
            const auto& bucket_keys = buckets[i];
            size_t n_i = bucket_keys.size();
            first_level[i].size = n_i * n_i;  // Quadratic size for collision-free
            
            bool success = false;
            for (int attempts = 0; attempts < 100 && !success; attempts++) {
                first_level[i].seed = gen();
                success = build_second_level(i, bucket_keys);
            }
        }
    }
}

bool BasicPerfectHash::build_second_level(size_t index, const std::vector<std::string>& bucket_keys) {
    auto& level = first_level[index];
    level.table.clear();
    level.table.resize(level.size);
    
    std::vector<bool> used(level.size, false);
    
    for (const auto& key : bucket_keys) {
        size_t pos = hash_function(key, level.seed) % level.size;
        if (used[pos]) {
            return false;  // Collision detected
        }
        used[pos] = true;
        level.table[pos] = key;
    }
    return true;
}

uint64_t BasicPerfectHash::hash(const std::string& key) const {
    size_t bucket = hash_function(key, primary_seed) % first_level.size();
    const auto& level = first_level[bucket];
    
    if (level.size == 0) return 0;
    
    size_t pos = hash_function(key, level.seed) % level.size;
    return bucket * 1000000 + pos;  // Unique ID
}

size_t BasicPerfectHash::getMemoryUsage() const {
    size_t total = sizeof(*this);
    for (const auto& level : first_level) {
        total += level.table.capacity() * sizeof(std::string);
        for (const auto& str : level.table) {
            total += str.capacity();
        }
    }
    return total;
}

void BasicPerfectHash::printStats() const {
    std::cout << "  First level buckets: " << first_level.size() << "\n";
    size_t total_second_level = 0;
    for (const auto& level : first_level) {
        total_second_level += level.size;
    }
    std::cout << "  Total second level slots: " << total_second_level << "\n";
    std::cout << "  Space overhead: " << (double)total_second_level / num_keys << "x\n";
}

} // namespace hashing

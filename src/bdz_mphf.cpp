#include "bdz_mphf.hpp"
#include "murmur_hash.hpp"
#include <random>
#include <queue>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <array>

namespace hashing {

void BDZ_MPHF::compute_three_hashes(const std::string& key, size_t& h0, size_t& h1, size_t& h2) const {
    // Use MurmurHash3 for high-quality, independent hash functions
    MurmurHash3::hash_triple(key, seeds[0], seeds[1], seeds[2], h0, h1, h2, table_size);
}

uint8_t BDZ_MPHF::get_g_value(size_t index) const {
    size_t byte_idx = index / 4;
    size_t bit_offset = (index % 4) * 2;
    return (g_values[byte_idx] >> bit_offset) & 0x03;
}

void BDZ_MPHF::set_g_value(size_t index, uint8_t value) {
    size_t byte_idx = index / 4;
    size_t bit_offset = (index % 4) * 2;
    g_values[byte_idx] &= ~(0x03 << bit_offset);
    g_values[byte_idx] |= (value & 0x03) << bit_offset;
}

bool BDZ_MPHF::build_graph_and_assign(const std::vector<std::string>& keys) {
    // Build 3-uniform hypergraph
    std::vector<std::vector<size_t>> edges(table_size);
    std::vector<std::array<size_t, 3>> key_edges(num_keys);
    
    for (size_t i = 0; i < num_keys; i++) {
        size_t h0, h1, h2;
        compute_three_hashes(keys[i], h0, h1, h2);
        key_edges[i][0] = h0;
        key_edges[i][1] = h1;
        key_edges[i][2] = h2;
        edges[h0].push_back(i);
        edges[h1].push_back(i);
        edges[h2].push_back(i);
    }
    
    // Peeling process: find vertices of degree 1
    std::queue<size_t> queue;
    std::vector<int> degree(table_size);
    
    for (size_t v = 0; v < table_size; v++) {
        degree[v] = static_cast<int>(edges[v].size());
        if (degree[v] == 1) {
            queue.push(v);
        }
    }
    
    std::vector<bool> visited_keys(num_keys, false);
    std::vector<size_t> ordering;
    
    while (!queue.empty()) {
        size_t v = queue.front();
        queue.pop();
        
        if (degree[v] == 0) continue;
        
        // Find the unvisited key
        size_t key_idx = table_size;
        for (size_t k : edges[v]) {
            if (!visited_keys[k]) {
                key_idx = k;
                break;
            }
        }
        
        if (key_idx == table_size) continue;
        
        visited_keys[key_idx] = true;
        ordering.push_back(key_idx);
        
        // Update degrees
        for (size_t j = 0; j < 3; j++) {
            size_t vertex = key_edges[key_idx][j];
            degree[vertex]--;
            if (degree[vertex] == 1) {
                queue.push(vertex);
            }
        }
    }
    
    // Check if all keys were processed
    if (ordering.size() != num_keys) {
        return false;  // Graph has cycles
    }
    
    // Assign g values in reverse order
    std::vector<int> assigned_values(table_size, -1);
    
    for (auto it = ordering.rbegin(); it != ordering.rend(); ++it) {
        size_t key_idx = *it;
        auto& edge = key_edges[key_idx];
        
        // Find which g-value to assign
        for (uint8_t g = 0; g < 3; g++) {
            size_t sum = 0;
            bool valid = true;
            
            for (size_t j = 0; j < 3; j++) {
                size_t v = edge[j];
                if (assigned_values[v] == -1) {
                    assigned_values[v] = g;
                    sum += g;
                    break;
                } else {
                    sum += assigned_values[v];
                }
            }
            
            if ((sum % 3) == (key_idx % 3)) {
                break;
            }
        }
    }
    
    // Store g-values
    for (size_t v = 0; v < table_size; v++) {
        if (assigned_values[v] != -1) {
            set_g_value(v, assigned_values[v]);
        }
    }
    
    return true;
}

double BDZ_MPHF::compute_chi_square(const std::vector<std::string>& keys) const {
    // Compute chi-square statistic for hash distribution quality
    // Tests if hash values are uniformly distributed across buckets
    std::vector<size_t> bucket_counts(num_keys, 0);

    for (const auto& key : keys) {
        uint64_t h = hash(key);
        if (h < num_keys) {
            bucket_counts[h]++;
        }
    }

    double expected = static_cast<double>(keys.size()) / num_keys;
    double chi_square = 0.0;

    for (size_t count : bucket_counts) {
        double diff = count - expected;
        chi_square += (diff * diff) / expected;
    }

    return chi_square;
}

void BDZ_MPHF::build(const std::vector<std::string>& keys) {
    num_keys = keys.size();
    table_size = static_cast<size_t>(1.23 * num_keys);  // BDZ requires ~1.23n space

    g_values.resize((table_size + 3) / 4, 0);

    std::random_device rd;
    std::mt19937_64 gen(rd());

    construction_stats = ConstructionStats();
    bool success = false;

    for (int attempt = 0; attempt < 100 && !success; attempt++) {
        construction_stats.attempts++;
        seeds[0] = gen();
        seeds[1] = gen();
        seeds[2] = gen();

        std::fill(g_values.begin(), g_values.end(), 0);
        success = build_graph_and_assign(keys);
    }

    construction_stats.success = success;

    if (!success) {
        std::cerr << "Warning: BDZ MPHF construction failed after 100 attempts\n";
    } else {
        // Compute chi-square statistic for the successful build
        construction_stats.chi_square = compute_chi_square(keys);
    }
}

uint64_t BDZ_MPHF::hash(const std::string& key) const {
    size_t h0, h1, h2;
    compute_three_hashes(key, h0, h1, h2);
    
    uint8_t g0 = get_g_value(h0);
    uint8_t g1 = get_g_value(h1);
    uint8_t g2 = get_g_value(h2);
    
    return (g0 + g1 + g2) % num_keys;
}

size_t BDZ_MPHF::getMemoryUsage() const {
    return sizeof(*this) + g_values.capacity();
}

void BDZ_MPHF::printStats() const {
    std::cout << "  Table size: " << table_size << " (" << (double)table_size / num_keys << "x keys)\n";
    std::cout << "  Memory: " << g_values.size() << " bytes ("
              << (g_values.size() * 8.0 / num_keys) << " bits/key)\n";
    std::cout << "  Theoretical minimum: ~1.44 bits/key\n";

    if (construction_stats.success) {
        std::cout << "  Construction attempts: " << construction_stats.attempts << "\n";
        std::cout << "  Chi-square statistic: " << std::fixed << std::setprecision(2)
                  << construction_stats.chi_square << " (lower is better)\n";
        // For reference: expected chi-square for uniform distribution is approximately (buckets - 1)
        std::cout << "  Expected χ² for uniform: ~" << (num_keys - 1) << "\n";
    }
}

} // namespace hashing

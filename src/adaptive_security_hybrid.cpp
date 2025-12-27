#include "adaptive_security_hybrid.hpp"
#include "murmur_hash.hpp"
#include <random>
#include <iostream>
#include <iomanip>
#include <cstring>
#include <algorithm>
#include <queue>
#include <array>

namespace hashing {

#define ROTL(x, b) (uint64_t)(((x) << (b)) | ((x) >> (64 - (b))))
#define ROTL32(x, b) (uint32_t)(((x) << (b)) | ((x) >> (32 - (b))))
#define SIPROUND \
    do { \
        v0 += v1; v1 = ROTL(v1, 13); v1 ^= v0; v0 = ROTL(v0, 32); \
        v2 += v3; v3 = ROTL(v3, 16); v3 ^= v2; \
        v0 += v3; v3 = ROTL(v3, 21); v3 ^= v0; \
        v2 += v1; v1 = ROTL(v1, 17); v1 ^= v2; v2 = ROTL(v2, 32); \
    } while(0)

uint64_t AdaptiveSecurityHybrid::siphash_stage(const std::string& key) const {
    const uint8_t* in = reinterpret_cast<const uint8_t*>(key.data());
    size_t inlen = key.size();
    
    uint64_t v0 = 0x736f6d6570736575ULL ^ sip_key0;
    uint64_t v1 = 0x646f72616e646f6dULL ^ sip_key1;
    uint64_t v2 = 0x6c7967656e657261ULL ^ sip_key0;
    uint64_t v3 = 0x7465646279746573ULL ^ sip_key1;
    uint64_t b = ((uint64_t)inlen) << 56;
    
    const uint8_t* end = in + inlen - (inlen % sizeof(uint64_t));
    for (; in != end; in += 8) {
        uint64_t m;
        std::memcpy(&m, in, sizeof(m));
        v3 ^= m;
        SIPROUND; SIPROUND;
        v0 ^= m;
    }
    
    const int left = inlen & 7;
    switch (left) {
        case 7: b |= ((uint64_t)in[6]) << 48; [[fallthrough]];
        case 6: b |= ((uint64_t)in[5]) << 40; [[fallthrough]];
        case 5: b |= ((uint64_t)in[4]) << 32; [[fallthrough]];
        case 4: b |= ((uint64_t)in[3]) << 24; [[fallthrough]];
        case 3: b |= ((uint64_t)in[2]) << 16; [[fallthrough]];
        case 2: b |= ((uint64_t)in[1]) << 8; [[fallthrough]];
        case 1: b |= ((uint64_t)in[0]); break;
        case 0: break;
    }
    
    v3 ^= b;
    SIPROUND; SIPROUND;
    v0 ^= b;
    v2 ^= 0xff;
    SIPROUND; SIPROUND; SIPROUND; SIPROUND;
    
    return v0 ^ v1 ^ v2 ^ v3;
}

uint64_t AdaptiveSecurityHybrid::blake3_stage(const std::string& key) const {
    const uint32_t IV[8] = {
        0x6A09E667, 0xBB67AE85, 0x3C6EF372, 0xA54FF53A,
        0x510E527F, 0x9B05688C, 0x1F83D9AB, 0x5BE0CD19
    };

    uint32_t state[8];
    std::memcpy(state, IV, sizeof(state));

    // Improved mixing with multiple rounds for better avalanche effect
    for (size_t i = 0; i < key.size(); i++) {
        uint32_t c = static_cast<uint32_t>(key[i]);
        size_t idx = i % 8;
        state[idx] ^= c;
        state[(idx + 1) % 8] ^= ROTL32(state[idx], 7);
        state[(idx + 2) % 8] ^= ROTL32(state[idx], 13);
        state[(idx + 3) % 8] ^= ROTL32(state[idx], 16);
    }

    // Final mixing round
    for (int i = 0; i < 8; i++) {
        state[i] = ROTL32(state[i], 11) ^ state[(i + 1) % 8];
    }

    // Use XOR of all state words
    uint64_t result = 0;
    for (int i = 0; i < 8; i++) {
        result ^= static_cast<uint64_t>(state[i]) << ((i % 2) * 32);
    }
    return result;
}

void AdaptiveSecurityHybrid::compute_mphf_hashes(uint64_t preprocessed, size_t& h0, size_t& h1, size_t& h2) const {
    // Use MurmurHash3 for high-quality, independent hash functions
    std::string preprocessed_str = std::to_string(preprocessed);
    MurmurHash3::hash_triple(preprocessed_str, mphf_seeds[0], mphf_seeds[1], mphf_seeds[2],
                             h0, h1, h2, table_size);
}

size_t AdaptiveSecurityHybrid::mphf_stage(uint64_t preprocessed) const {
    size_t h0, h1, h2;
    compute_mphf_hashes(preprocessed, h0, h1, h2);
    
    auto get_g = [this](size_t v) -> uint8_t {
        size_t byte_idx = v / 4;
        size_t bit_offset = (v % 4) * 2;
        return (g_table[byte_idx] >> bit_offset) & 0x03;
    };
    
    uint8_t g0 = get_g(h0);
    uint8_t g1 = get_g(h1);
    uint8_t g2 = get_g(h2);
    
    return (g0 + g1 + g2) % num_keys;
}

bool AdaptiveSecurityHybrid::should_verify(const std::string& key) const {
    if (security_level >= 5) return true;  // High: always verify
    if (security_level <= 1) return false; // Low: never verify
    
    // Medium: sample based on key hash
    uint64_t h = siphash_stage(key);
    return (h & 0x0F) == 0; // 1/16 sampling
}

bool AdaptiveSecurityHybrid::build_mphf(const std::vector<std::string>& keys) {
    std::vector<std::vector<size_t>> edges(table_size);
    std::vector<std::array<size_t, 3>> key_edges(num_keys);
    std::vector<uint64_t> preprocessed_keys(num_keys);
    
    for (size_t i = 0; i < num_keys; i++) {
        preprocessed_keys[i] = siphash_stage(keys[i]);
        size_t h0, h1, h2;
        compute_mphf_hashes(preprocessed_keys[i], h0, h1, h2);
        
        key_edges[i][0] = h0;
        key_edges[i][1] = h1;
        key_edges[i][2] = h2;
        edges[h0].push_back(i);
        edges[h1].push_back(i);
        edges[h2].push_back(i);
    }
    
    std::queue<size_t> queue;
    std::vector<int> degree(table_size);
    
    for (size_t v = 0; v < table_size; v++) {
        degree[v] = static_cast<int>(edges[v].size());
        if (degree[v] == 1) queue.push(v);
    }
    
    std::vector<bool> visited(num_keys, false);
    std::vector<size_t> ordering;
    
    while (!queue.empty()) {
        size_t v = queue.front();
        queue.pop();
        
        if (degree[v] == 0) continue;
        
        size_t key_idx = table_size;
        for (size_t k : edges[v]) {
            if (!visited[k]) {
                key_idx = k;
                break;
            }
        }
        
        if (key_idx == table_size) continue;
        
        visited[key_idx] = true;
        ordering.push_back(key_idx);
        
        for (size_t j = 0; j < 3; j++) {
            size_t vertex = key_edges[key_idx][j];
            degree[vertex]--;
            if (degree[vertex] == 1) queue.push(vertex);
        }
    }
    
    if (ordering.size() != num_keys) return false;

    std::vector<int> assigned(table_size, -1);
    g_table.clear();
    g_table.resize((table_size + 3) / 4, 0);
    
    for (auto it = ordering.rbegin(); it != ordering.rend(); ++it) {
        size_t key_idx = *it;
        auto& edge = key_edges[key_idx];
        
        for (uint8_t g = 0; g < 3; g++) {
            size_t sum = 0;
            for (size_t j = 0; j < 3; j++) {
                size_t v = edge[j];
                if (assigned[v] == -1) {
                    assigned[v] = g;
                    size_t byte_idx = v / 4;
                    size_t bit_offset = (v % 4) * 2;
                    g_table[byte_idx] |= (g & 0x03) << bit_offset;
                    sum += g;
                    break;
                } else {
                    sum += assigned[v];
                }
            }
            if ((sum % 3) == (key_idx % 3)) break;
        }
    }
    
    return true;
}

double AdaptiveSecurityHybrid::compute_chi_square(const std::vector<std::string>& keys) const {
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

void AdaptiveSecurityHybrid::build(const std::vector<std::string>& keys) {
    num_keys = keys.size();
    table_size = static_cast<size_t>(1.23 * num_keys);

    std::random_device rd;
    std::mt19937_64 gen(rd());

    sip_key0 = gen();
    sip_key1 = gen();
    mphf_seeds[0] = gen();
    mphf_seeds[1] = gen();
    mphf_seeds[2] = gen();

    // Initialize construction statistics
    construction_stats = ConstructionStats();

    bool success = false;
    for (int attempt = 0; attempt < 100 && !success; attempt++) {
        construction_stats.attempts++;
        success = build_mphf(keys);
        if (!success) {
            mphf_seeds[0] = gen();
            mphf_seeds[1] = gen();
            mphf_seeds[2] = gen();
        }
    }

    construction_stats.success = success;

    // Build verification fingerprints
    blake3_fingerprints.resize(num_keys);
    for (size_t i = 0; i < num_keys; i++) {
        blake3_fingerprints[i] = blake3_stage(keys[i]);
    }

    // Compute chi-square for successful builds
    if (success) {
        construction_stats.chi_square = compute_chi_square(keys);
    }
}

uint64_t AdaptiveSecurityHybrid::hash(const std::string& key) const {
    uint64_t preprocessed = siphash_stage(key);
    size_t index = mphf_stage(preprocessed);
    
    // Conditional verification based on security level
    if (should_verify(key)) {
        uint64_t expected = blake3_fingerprints[index];
        uint64_t actual = blake3_stage(key);
        if (expected != actual) {
            return UINT64_MAX; // Verification failure
        }
    }
    
    return index;
}

size_t AdaptiveSecurityHybrid::getMemoryUsage() const {
    return g_table.size() + blake3_fingerprints.size() * sizeof(uint64_t) + 
           sizeof(*this) + 1; // +1 for security_level
}

void AdaptiveSecurityHybrid::printStats() const {
    const char* level_str[] = {"Very Low", "Low", "Low-Med", "Medium",
                               "Med-High", "High", "Very High", "Maximum"};
    std::cout << "  Architecture: SipHash + BDZ-MPHF + Conditional-BLAKE3\n";
    std::cout << "  Security Level: " << static_cast<int>(security_level)
              << " (" << level_str[security_level] << ")\n";
    std::cout << "  Verification Rate: ";
    if (security_level >= 5) std::cout << "100% (always)\n";
    else if (security_level <= 1) std::cout << "0% (never)\n";
    else std::cout << "6.25% (sampled)\n";
    std::cout << "  MPHF Space: " << g_table.size() << " bytes\n";
    std::cout << "  Fingerprints: " << blake3_fingerprints.size() * 8 << " bytes\n";

    if (construction_stats.success) {
        std::cout << "  Construction attempts: " << construction_stats.attempts << "\n";
        std::cout << "  Chi-square statistic: " << std::fixed << std::setprecision(2)
                  << construction_stats.chi_square << " (lower is better)\n";
        std::cout << "  Expected χ² for uniform: ~" << (num_keys - 1) << "\n";
    }
}

std::string AdaptiveSecurityHybrid::getName() const {
    const char* level_str[] = {"VeryLow", "Low", "LowMed", "Med", 
                               "MedHigh", "High", "VeryHigh", "Max"};
    return std::string("Adaptive Security Hybrid (Level ") + 
           std::to_string(security_level) + "-" + level_str[security_level] + ")";
}

} // namespace hashing

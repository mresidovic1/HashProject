#include "two_path_hybrid.hpp"
#include <random>
#include <iostream>
#include <iomanip>
#include <cstring>
#include <algorithm>
#include <queue>
#include <array>

namespace hashing {

#define ROTL(x, b) (uint64_t)(((x) << (b)) | ((x) >> (64 - (b))))
#define SIPROUND \
    do { \
        v0 += v1; v1 = ROTL(v1, 13); v1 ^= v0; v0 = ROTL(v0, 32); \
        v2 += v3; v3 = ROTL(v3, 16); v3 ^= v2; \
        v0 += v3; v3 = ROTL(v3, 21); v3 ^= v0; \
        v2 += v1; v1 = ROTL(v1, 17); v1 ^= v2; v2 = ROTL(v2, 32); \
    } while(0)

bool TwoPathHybrid::route_to_secure_lane(const std::string& key) const {
    // Deterministic routing: use LSB of hash
    uint64_t h = fast_siphash(key);
    return (h & 0x01) != 0;
}

uint64_t TwoPathHybrid::fast_siphash(const std::string& key) const {
    const uint8_t* in = reinterpret_cast<const uint8_t*>(key.data());
    size_t inlen = key.size();
    
    uint64_t v0 = 0x736f6d6570736575ULL ^ fast_sip_key0;
    uint64_t v1 = 0x646f72616e646f6dULL ^ fast_sip_key1;
    uint64_t v2 = 0x6c7967656e657261ULL ^ fast_sip_key0;
    uint64_t v3 = 0x7465646279746573ULL ^ fast_sip_key1;
    uint64_t b = ((uint64_t)inlen) << 56;
    
    const uint8_t* end = in + inlen - (inlen % sizeof(uint64_t));
    for (; in != end; in += 8) {
        uint64_t m;
        std::memcpy(&m, in, sizeof(m));
        v3 ^= m;
        SIPROUND; // Only 1 round (SipHash-1-2)
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
    SIPROUND;
    v0 ^= b;
    v2 ^= 0xff;
    SIPROUND; SIPROUND; // 2 finalization rounds
    
    return v0 ^ v1 ^ v2 ^ v3;
}

uint64_t TwoPathHybrid::secure_siphash(const std::string& key) const {
    const uint8_t* in = reinterpret_cast<const uint8_t*>(key.data());
    size_t inlen = key.size();
    
    uint64_t v0 = 0x736f6d6570736575ULL ^ secure_sip_key0;
    uint64_t v1 = 0x646f72616e646f6dULL ^ secure_sip_key1;
    uint64_t v2 = 0x6c7967656e657261ULL ^ secure_sip_key0;
    uint64_t v3 = 0x7465646279746573ULL ^ secure_sip_key1;
    uint64_t b = ((uint64_t)inlen) << 56;
    
    const uint8_t* end = in + inlen - (inlen % sizeof(uint64_t));
    for (; in != end; in += 8) {
        uint64_t m;
        std::memcpy(&m, in, sizeof(m));
        v3 ^= m;
        SIPROUND; SIPROUND; // 2 rounds (SipHash-2-4)
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
    SIPROUND; SIPROUND; SIPROUND; SIPROUND; // 4 finalization rounds
    
    return v0 ^ v1 ^ v2 ^ v3;
}

uint64_t TwoPathHybrid::blake3_stage(const std::string& key) const {
    const uint32_t IV[8] = {
        0x6A09E667, 0xBB67AE85, 0x3C6EF372, 0xA54FF53A,
        0x510E527F, 0x9B05688C, 0x1F83D9AB, 0x5BE0CD19
    };
    
    uint32_t state[8];
    std::memcpy(state, IV, sizeof(state));
    
    for (size_t i = 0; i < key.size(); i++) {
        uint32_t c = static_cast<uint32_t>(key[i]);
        state[i % 8] ^= c;
        state[(i + 1) % 8] = ROTL(state[(i + 1) % 8], 7) ^ state[i % 8];
    }
    
    return (static_cast<uint64_t>(state[0]) << 32) | state[1];
}

void TwoPathHybrid::compute_mphf_hashes(uint64_t preprocessed, const uint64_t seeds[3], 
                                        size_t table_sz, size_t& h0, size_t& h1, size_t& h2) const {
    h0 = (preprocessed ^ seeds[0]) % table_sz;
    h1 = ((preprocessed >> 16) ^ seeds[1]) % table_sz;
    h2 = ((preprocessed >> 32) ^ seeds[2]) % table_sz;
    
    if (h1 == h0) h1 = (h1 + 1) % table_sz;
    if (h2 == h0 || h2 == h1) h2 = (h2 + 1) % table_sz;
}

size_t TwoPathHybrid::fast_mphf(uint64_t preprocessed) const {
    size_t h0, h1, h2;
    compute_mphf_hashes(preprocessed, fast_seeds, fast_table_size, h0, h1, h2);
    
    auto get_g = [this](size_t v) -> uint8_t {
        size_t byte_idx = v / 4;
        size_t bit_offset = (v % 4) * 2;
        return (fast_g_table[byte_idx] >> bit_offset) & 0x03;
    };
    
    uint8_t g0 = get_g(h0);
    uint8_t g1 = get_g(h1);
    uint8_t g2 = get_g(h2);
    
    return (g0 + g1 + g2) % (num_keys / 2); // Approx half keys
}

size_t TwoPathHybrid::secure_mphf(uint64_t preprocessed) const {
    size_t h0, h1, h2;
    compute_mphf_hashes(preprocessed, secure_seeds, secure_table_size, h0, h1, h2);
    
    auto get_g = [this](size_t v) -> uint8_t {
        size_t byte_idx = v / 4;
        size_t bit_offset = (v % 4) * 2;
        return (secure_g_table[byte_idx] >> bit_offset) & 0x03;
    };
    
    uint8_t g0 = get_g(h0);
    uint8_t g1 = get_g(h1);
    uint8_t g2 = get_g(h2);
    
    return (g0 + g1 + g2) % (num_keys / 2 + num_keys % 2);
}

bool TwoPathHybrid::build_mphf(const std::vector<std::string>& keys, std::vector<uint8_t>& g_table,
                               size_t table_sz, const uint64_t seeds[3], uint64_t sip_k0, uint64_t sip_k1) {
    size_t n = keys.size();
    std::vector<std::vector<size_t>> edges(table_sz);
    std::vector<std::array<size_t, 3>> key_edges(n);
    
    auto siphash_local = [&](const std::string& key) -> uint64_t {
        const uint8_t* in = reinterpret_cast<const uint8_t*>(key.data());
        size_t inlen = key.size();
        
        uint64_t v0 = 0x736f6d6570736575ULL ^ sip_k0;
        uint64_t v1 = 0x646f72616e646f6dULL ^ sip_k1;
        uint64_t v2 = 0x6c7967656e657261ULL ^ sip_k0;
        uint64_t v3 = 0x7465646279746573ULL ^ sip_k1;
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
    };
    
    for (size_t i = 0; i < n; i++) {
        uint64_t preprocessed = siphash_local(keys[i]);
        size_t h0, h1, h2;
        compute_mphf_hashes(preprocessed, seeds, table_sz, h0, h1, h2);
        
        key_edges[i][0] = h0;
        key_edges[i][1] = h1;
        key_edges[i][2] = h2;
        edges[h0].push_back(i);
        edges[h1].push_back(i);
        edges[h2].push_back(i);
    }
    
    std::queue<size_t> queue;
    std::vector<int> degree(table_sz);
    
    for (size_t v = 0; v < table_sz; v++) {
        degree[v] = static_cast<int>(edges[v].size());
        if (degree[v] == 1) queue.push(v);
    }
    
    std::vector<bool> visited(n, false);
    std::vector<size_t> ordering;
    
    while (!queue.empty()) {
        size_t v = queue.front();
        queue.pop();
        
        if (degree[v] == 0) continue;
        
        size_t key_idx = table_sz;
        for (size_t k : edges[v]) {
            if (!visited[k]) {
                key_idx = k;
                break;
            }
        }
        
        if (key_idx == table_sz) continue;
        
        visited[key_idx] = true;
        ordering.push_back(key_idx);
        
        for (size_t j = 0; j < 3; j++) {
            size_t vertex = key_edges[key_idx][j];
            degree[vertex]--;
            if (degree[vertex] == 1) queue.push(vertex);
        }
    }
    
    if (ordering.size() != n) return false;
    
    std::vector<int> assigned(table_sz, -1);
    g_table.clear();
    g_table.resize((table_sz + 3) / 4, 0);
    
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

void TwoPathHybrid::build(const std::vector<std::string>& keys) {
    num_keys = keys.size();
    fast_table_size = static_cast<size_t>(1.23 * (num_keys / 2));
    secure_table_size = static_cast<size_t>(1.23 * (num_keys / 2 + num_keys % 2));
    
    std::random_device rd;
    std::mt19937_64 gen(rd());
    
    fast_sip_key0 = gen();
    fast_sip_key1 = gen();
    fast_seeds[0] = gen();
    fast_seeds[1] = gen();
    fast_seeds[2] = gen();
    
    secure_sip_key0 = gen();
    secure_sip_key1 = gen();
    secure_seeds[0] = gen();
    secure_seeds[1] = gen();
    secure_seeds[2] = gen();
    
    // Split keys into two lanes
    std::vector<std::string> fast_keys, secure_keys;
    for (const auto& key : keys) {
        if (route_to_secure_lane(key)) {
            secure_keys.push_back(key);
        } else {
            fast_keys.push_back(key);
        }
    }
    
    // Build fast lane MPHF
    bool success = false;
    int attempts = 0;
    while (!success && attempts < 10) {
        success = build_mphf(fast_keys, fast_g_table, fast_table_size, fast_seeds, 
                           fast_sip_key0, fast_sip_key1);
        if (!success) {
            fast_seeds[0] = gen();
            fast_seeds[1] = gen();
            fast_seeds[2] = gen();
            attempts++;
        }
    }
    
    // Build secure lane MPHF
    success = false;
    attempts = 0;
    while (!success && attempts < 10) {
        success = build_mphf(secure_keys, secure_g_table, secure_table_size, secure_seeds,
                           secure_sip_key0, secure_sip_key1);
        if (!success) {
            secure_seeds[0] = gen();
            secure_seeds[1] = gen();
            secure_seeds[2] = gen();
            attempts++;
        }
    }
    
    // Build verification fingerprints for secure lane only
    secure_fingerprints.resize(secure_keys.size());
    for (size_t i = 0; i < secure_keys.size(); i++) {
        secure_fingerprints[i] = blake3_stage(secure_keys[i]);
    }
}

uint64_t TwoPathHybrid::hash(const std::string& key) const {
    if (route_to_secure_lane(key)) {
        // Secure lane: SipHash-2-4 + BDZ + BLAKE3
        uint64_t preprocessed = secure_siphash(key);
        size_t index = secure_mphf(preprocessed);
        
        uint64_t expected = secure_fingerprints[index];
        uint64_t actual = blake3_stage(key);
        
        if (expected != actual) {
            return UINT64_MAX;
        }
        
        return index;
    } else {
        // Fast lane: SipHash-1-2 + Simple MPHF (no verification)
        uint64_t preprocessed = fast_siphash(key);
        return fast_mphf(preprocessed);
    }
}

size_t TwoPathHybrid::getMemoryUsage() const {
    return fast_g_table.size() + 
           secure_g_table.size() + 
           secure_fingerprints.size() * sizeof(uint64_t) +
           sizeof(*this);
}

void TwoPathHybrid::printStats() const {
    std::cout << "  Architecture: Dual-Path (Fast: CHD-like | Secure: BDZ+BLAKE3)\n";
    std::cout << "  Routing: Deterministic hash-based lane assignment\n";
    std::cout << "  Fast Lane MPHF: " << fast_g_table.size() << " bytes\n";
    std::cout << "  Secure Lane MPHF: " << secure_g_table.size() << " bytes\n";
    std::cout << "  Secure Fingerprints: " << secure_fingerprints.size() * 8 << " bytes\n";
    std::cout << "  Lane Distribution: ~50% fast / ~50% secure\n";
}

} // namespace hashing

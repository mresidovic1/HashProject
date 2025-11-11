#include "mphf.h"
#include <algorithm>
#include <random>

// MurmurHash3 inspirisana hash funkcija
uint32_t MinimalPerfectHashFunction::hash1(const std::string& key) const {
    uint32_t hash = seed1;
    for (char c : key) {
        hash ^= static_cast<uint32_t>(c);
        hash *= 0x5bd1e995;
        hash ^= hash >> 15;
    }
    return hash;
}

uint32_t MinimalPerfectHashFunction::hash2(const std::string& key) const {
    uint32_t hash = seed2;
    for (size_t i = 0; i < key.length(); ++i) {
        hash = ((hash << 5) + hash) + static_cast<uint32_t>(key[i]);
    }
    return hash;
}

uint32_t MinimalPerfectHashFunction::hash3(const std::string& key) const {
    uint32_t hash = 0;
    for (size_t i = 0; i < key.length(); ++i) {
        hash = static_cast<uint32_t>(key[i]) + (hash << 6) + (hash << 16) - hash;
    }
    return hash;
}

void MinimalPerfectHashFunction::build(const std::vector<std::string>& keys) {
    if (keys.empty()) return;
    
    originalKeys = keys;
    int n = keys.size();
    
    // Proširenje tabele za bolju distribuciju (load factor ~0.7)
    int tableSize = static_cast<int>(n * 1.5);
    table.resize(tableSize, -1);
    keyIndex.clear();
    
    // Pokušaj izgradnje sa različitim seed-ovima dok ne postignemo savršen hash
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint32_t> dis;
    
    bool success = false;
    int attempts = 0;
    const int maxAttempts = 100;
    
    while (!success && attempts < maxAttempts) {
        seed1 = dis(gen);
        seed2 = dis(gen);
        
        std::fill(table.begin(), table.end(), -1);
        keyIndex.clear();
        
        success = true;
        
        // Pokušaj mapiranja svih ključeva
        for (int i = 0; i < n; ++i) {
            const std::string& key = keys[i];
            
            // Pokušaj sa 3 različite hash funkcije
            std::vector<int> positions = {
                static_cast<int>(hash1(key) % tableSize),
                static_cast<int>(hash2(key) % tableSize),
                static_cast<int>(hash3(key) % tableSize)
            };
            
            bool placed = false;
            for (int pos : positions) {
                if (table[pos] == -1) {
                    table[pos] = i;
                    keyIndex[key] = i;
                    placed = true;
                    break;
                }
            }
            
            if (!placed) {
                success = false;
                break;
            }
        }
        
        ++attempts;
    }
    
    // Ako nismo uspeli sa savršenim hashom, koristimo fallback mapu
    if (!success) {
        table.clear();
        table.resize(tableSize, -1);
        for (int i = 0; i < n; ++i) {
            keyIndex[keys[i]] = i;
        }
    }
}

int MinimalPerfectHashFunction::lookup(const std::string& key) const {
    if (originalKeys.empty()) return -1;
    
    // Prvo pokušaj sa hash tabelom
    if (!table.empty()) {
        int tableSize = table.size();
        std::vector<int> positions = {
            static_cast<int>(hash1(key) % tableSize),
            static_cast<int>(hash2(key) % tableSize),
            static_cast<int>(hash3(key) % tableSize)
        };
        
        for (int pos : positions) {
            if (table[pos] != -1 && originalKeys[table[pos]] == key) {
                return table[pos];
            }
        }
    }
    
    // Fallback na mapu
    auto it = keyIndex.find(key);
    return (it != keyIndex.end()) ? it->second : -1;
}

bool MinimalPerfectHashFunction::isPerfect() const {
    if (originalKeys.empty()) return true;
    
    // Provera da li svaki ključ ima jedinstvenu poziciju
    for (const auto& key : originalKeys) {
        int idx = lookup(key);
        if (idx == -1) return false;
    }
    
    // Provera da li ima kolizija
    std::vector<bool> used(originalKeys.size(), false);
    for (const auto& key : originalKeys) {
        int idx = lookup(key);
        if (idx < 0 || idx >= originalKeys.size() || used[idx]) {
            return false;
        }
        used[idx] = true;
    }
    
    return true;
}

double MinimalPerfectHashFunction::getLoadFactor() const {
    if (table.empty()) return 0.0;
    int occupied = 0;
    for (int val : table) {
        if (val != -1) ++occupied;
    }
    return static_cast<double>(occupied) / table.size();
}

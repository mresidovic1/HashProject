#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>

// Minimal Perfect Hash Function (MPHF)
// Napredna implementacija sa hash familijama i collision resolution
class MinimalPerfectHashFunction {
public:
    MinimalPerfectHashFunction() : seed1(0x12345678), seed2(0x9ABCDEF0) {}
    
    // Izgradnja MPHF strukture za dati skup ključeva
    void build(const std::vector<std::string>& keys);
    
    // Pretraga ključa - vraća jedinstveni indeks ili -1 ako ključ ne postoji
    int lookup(const std::string& key) const;
    
    // Provera da li je hash zaista savršen (nema kolizija)
    bool isPerfect() const;
    
    // Metrike efikasnosti
    double getLoadFactor() const;
    int getTableSize() const { return static_cast<int>(table.size()); }
    int getKeyCount() const { return static_cast<int>(originalKeys.size()); }
    
private:
    uint32_t hash1(const std::string& key) const;
    uint32_t hash2(const std::string& key) const;
    uint32_t hash3(const std::string& key) const;
    
    std::vector<int> table;              // Hash tabela
    std::vector<std::string> originalKeys; // Originalni ključevi
    uint32_t seed1, seed2;               // Seed-ovi za hash funkcije
    std::unordered_map<std::string, int> keyIndex; // Mapa ključ -> indeks
};

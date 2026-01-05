#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>
#include <memory>
#include <random>
#include <iomanip>

// Include svih 9 algoritama
#include "basic_perfect_hash.hpp"
#include "basic_crypto_hash.hpp"
#include "bdz_mphf.hpp"
#include "ultimate_hybrid.hpp"
#include "adaptive_security_hybrid.hpp"
#include "parallel_verification_hybrid.hpp"
#include "cache_partitioned_hybrid.hpp"
#include "ultra_low_memory_hybrid.hpp"
#include "two_path_hybrid.hpp"

using namespace hashing;

// Generisanje uniformnih ključeva
std::vector<std::string> generateKeys(size_t n) {
    std::vector<std::string> keys;
    keys.reserve(n);
    std::mt19937_64 rng(42);
    
    for (size_t i = 0; i < n; i++) {
        keys.push_back("key_" + std::to_string(rng()) + "_" + std::to_string(i));
    }
    return keys;
}

int main() {
    std::cout << "=================================================================\n";
    std::cout << "         HASH ALGORITHM THROUGHPUT BENCHMARK\n";
    std::cout << "=================================================================\n\n";
    
    // Test veličine: 1k, 10k, 100k, 1M, 5M, 10M
    std::vector<size_t> key_sizes = {1000, 10000, 100000, 1000000, 5000000, 10000000};
    
    // CSV fajl za izlaz
    std::ofstream csv("benchmark_results.csv");
    csv << "Algorithm,Num_Keys,Insert_Ops_Per_Sec,Lookup_Ops_Per_Sec,Memory_Usage_MB,Efficiency_Ops_Per_MB\n";
    
    for (size_t size : key_sizes) {
        std::cout << "\n=== Testiranje sa " << size << " kljuceva ===\n";
        
        // Generisanje ključeva
        std::cout << "Generisanje kljuceva...\n";
        std::vector<std::string> keys = generateKeys(size);
        
        // Testiranje svih 9 algoritama
        std::vector<std::unique_ptr<HashModel>> algorithms;
        algorithms.push_back(std::make_unique<BasicPerfectHash>());
        algorithms.push_back(std::make_unique<BasicCryptoHash>());
        algorithms.push_back(std::make_unique<BDZ_MPHF>());
        algorithms.push_back(std::make_unique<UltimateHybridHash>());
        algorithms.push_back(std::make_unique<AdaptiveSecurityHybrid>());
        algorithms.push_back(std::make_unique<ParallelVerificationHybrid>());
        algorithms.push_back(std::make_unique<CachePartitionedHybrid>());
        algorithms.push_back(std::make_unique<UltraLowMemoryHybrid>());
        algorithms.push_back(std::make_unique<TwoPathHybrid>());
        
        int count = 0;
        for (auto& algo : algorithms) {
            count++;
            std::cout << "[" << count << "/9] " << std::setw(40) << std::left 
                      << algo->getName() << " ... ";
            std::cout.flush();
            
            try {
                // === BULK INSERT ===
                auto insert_start = std::chrono::high_resolution_clock::now();
                algo->build(keys);
                auto insert_end = std::chrono::high_resolution_clock::now();
                
                double insert_sec = std::chrono::duration<double>(insert_end - insert_start).count();
                double insert_mops = (size / insert_sec) / 1e6;
                
                // === BULK LOOKUP ===
                auto lookup_start = std::chrono::high_resolution_clock::now();
                for (const auto& key : keys) {
                    volatile uint64_t hash_val = algo->hash(key);
                    (void)hash_val;
                }
                auto lookup_end = std::chrono::high_resolution_clock::now();
                
                double lookup_sec = std::chrono::duration<double>(lookup_end - lookup_start).count();
                double lookup_mops = (size / lookup_sec) / 1e6;
                
                // === MEMORY ===
                double memory_mb = algo->getMemoryUsage() / (1024.0 * 1024.0);
                
                // === EFFICIENCY: Lookup throughput per MB ===
                double efficiency = (memory_mb > 0.0) ? (lookup_mops / memory_mb) : 0.0;
                
                // Ispis
                std::cout << "Insert: " << std::fixed << std::setprecision(2) 
                          << insert_mops << " Mops/s, "
                          << "Lookup: " << lookup_mops << " Mops/s, "
                          << "Mem: " << memory_mb << " MB, "
                          << "Eff: " << efficiency << " Ops/MB\n";
                
                // Upis u CSV
                csv << std::fixed << std::setprecision(6)
                    << algo->getName() << ","
                    << size << ","
                    << insert_mops << ","
                    << lookup_mops << ","
                    << std::setprecision(3) << memory_mb << ","
                    << std::setprecision(6) << efficiency << "\n";
                csv.flush(); // Flush nakon svakog testa
                
            } catch (const std::exception& e) {
                std::cout << "FAILED: " << e.what() << "\n";
                
                // Upisivanje neuspelog testa
                csv << algo->getName() << ","
                    << size << ","
                    << "0.0,0.0,0.0,0.0\n";
                csv.flush();
            }
        }
    }
    
    csv.close();
    
    std::cout << "\n=================================================================\n";
    std::cout << "CSV fajl kreiran: benchmark_results.csv\n";
    std::cout << "Spreman za Python vizuelizaciju!\n";
    std::cout << "=================================================================\n";
    
    return 0;
}

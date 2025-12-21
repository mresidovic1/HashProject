#include <iostream>
#include <iomanip>
#include <vector>
#include <chrono>
#include <memory>

#include "basic_perfect_hash.hpp"
#include "basic_crypto_hash.hpp"
#include "bdz_mphf.hpp"
#include "ultimate_hybrid.hpp"

using namespace hashing;

std::vector<std::string> generate_test_keys(size_t n) {
    std::vector<std::string> keys;
    keys.reserve(n);
    
    for (size_t i = 0; i < n; i++) {
        keys.push_back("key_" + std::to_string(i) + "_test_data_" + std::to_string(i * 7919));
    }
    
    return keys;
}

void print_header(const std::string& title) {
    std::cout << "\n" << std::string(80, '=') << "\n";
    std::cout << title << "\n";
    std::cout << std::string(80, '=') << "\n\n";
}

void benchmark_model(HashModel& model, const std::vector<std::string>& keys) {
    std::cout << "Model: " << model.getName() << "\n";
    std::cout << std::string(60, '-') << "\n";
    
    // Build phase
    auto build_start = std::chrono::high_resolution_clock::now();
    model.build(keys);
    auto build_end = std::chrono::high_resolution_clock::now();
    
    double build_ms = std::chrono::duration<double, std::milli>(build_end - build_start).count();
    
    // Lookup phase
    auto lookup_start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < std::min(keys.size(), size_t(10000)); i++) {
        volatile uint64_t h = model.hash(keys[i]);
        (void)h;
    }
    auto lookup_end = std::chrono::high_resolution_clock::now();
    
    double lookup_ns = std::chrono::duration<double, std::nano>(lookup_end - lookup_start).count() / 
                       std::min(keys.size(), size_t(10000));
    
    std::cout << std::fixed << std::setprecision(3);
    std::cout << "  Build time: " << build_ms << " ms\n";
    std::cout << "  Avg lookup: " << lookup_ns << " ns\n";
    std::cout << "  Memory: " << model.getMemoryUsage() << " bytes\n";
    std::cout << "  Bits/key: " << (model.getMemoryUsage() * 8.0 / keys.size()) << "\n";
    
    model.printStats();
    std::cout << "\n";
}

int main() {
    print_header("ADVANCED HASHING MODELS - COMPREHENSIVE EVALUATION");
    
    std::cout << "Course: Advanced Algorithms and Data Structures\n";
    std::cout << "Project: Hybrid Perfect and Cryptographic Hashing Models\n\n";
    
    const size_t NUM_KEYS = 10000;
    auto keys = generate_test_keys(NUM_KEYS);
    
    std::cout << "Dataset: " << NUM_KEYS << " synthetic keys\n";
    
    // Baseline Models
    print_header("1. BASELINE MODELS");
    
    BasicPerfectHash basic_perfect;
    benchmark_model(basic_perfect, keys);
    
    BasicCryptoHash basic_crypto;
    benchmark_model(basic_crypto, keys);
    
    // Advanced Perfect Hash Models
    print_header("2. ADVANCED PERFECT HASH MODELS");
    
    BDZ_MPHF bdz;
    benchmark_model(bdz, keys);
    
    // Ultimate Hybrid Model
    print_header("3. ULTIMATE HYBRID MODEL");
    
    UltimateHybridHash ultimate;
    benchmark_model(ultimate, keys);
    
    if (auto* ult_ptr = dynamic_cast<UltimateHybridHash*>(&ultimate)) {
        ult_ptr->printSecurityAnalysis();
        
        std::cout << "\n=== Performance Metrics ===\n";
        std::cout << "  Average probe length: " << ult_ptr->getAverageProbeLength() << "\n";
        std::cout << "  Cache efficiency: " << ult_ptr->getCacheEfficiency() << "%\n";
    }
    
    // Comparison Summary
    print_header("4. COMPARATIVE SUMMARY");
    
    std::cout << std::left << std::setw(35) << "Model"
              << std::setw(15) << "Memory (bits/key)"
              << std::setw(15) << "Lookup (ns)"
              << "Properties\n";
    std::cout << std::string(80, '-') << "\n";
    
    std::cout << std::setw(35) << "Basic Perfect (FKS)"
              << std::setw(15) << "~high" << std::setw(15) << "~fast"
              << "Collision-free, high space\n";
              
    std::cout << std::setw(35) << "Basic Crypto (SHA-256)"
              << std::setw(15) << "0" << std::setw(15) << "~slow"
              << "Secure, no storage\n";
              
    std::cout << std::setw(35) << "BDZ MPHF"
              << std::setw(15) << "~2-3" << std::setw(15) << "~fast"
              << "Minimal space, static\n";
              
    std::cout << std::setw(35) << "Ultimate Hybrid"
              << std::setw(15) << "~10-15" << std::setw(15) << "~medium"
              << "Secure + Perfect + Fast\n";
    
    print_header("EVALUATION COMPLETE");
    
    return 0;
}

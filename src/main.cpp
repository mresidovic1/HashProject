#include <iostream>
#include <iomanip>
#include <vector>
#include <chrono>
#include <memory>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <random>
#include <cstdio>

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

std::vector<std::string> generate_test_keys(size_t n) {
    std::vector<std::string> keys;
    keys.reserve(n);

    for (size_t i = 0; i < n; i++) {
        keys.push_back("key_" + std::to_string(i) + "_test_data_" + std::to_string(i * 7919));
    }

    return keys;
}

std::vector<std::string> generate_negative_keys(size_t n) {
    // Generate keys that are NOT in the original dataset
    std::vector<std::string> keys;
    keys.reserve(n);

    for (size_t i = 0; i < n; i++) {
        keys.push_back("negative_key_" + std::to_string(i) + "_not_in_set_" + std::to_string(i * 9973));
    }

    return keys;
}

std::vector<std::string> generate_uuid_keys(size_t n) {
    // Generate UUID-like keys (realistic pattern: 8-4-4-4-12 hex digits)
    std::vector<std::string> keys;
    keys.reserve(n);

    std::random_device rd;
    std::mt19937_64 gen(rd());

    for (size_t i = 0; i < n; i++) {
        uint64_t part1 = gen();
        uint64_t part2 = gen();

        char uuid[37];
        snprintf(uuid, sizeof(uuid), "%08llx-%04llx-%04llx-%04llx-%012llx",
                 (part1 >> 32) & 0xFFFFFFFF,
                 (part1 >> 16) & 0xFFFF,
                 part1 & 0xFFFF,
                 (part2 >> 48) & 0xFFFF,
                 part2 & 0xFFFFFFFFFFFFULL);

        keys.push_back(std::string(uuid));
    }

    return keys;
}

std::vector<std::string> generate_url_keys(size_t n) {
    // Generate URL-like keys (realistic web patterns)
    std::vector<std::string> keys;
    keys.reserve(n);

    const std::vector<std::string> domains = {
        "example.com", "test.org", "api.service.io", "cdn.website.net",
        "app.platform.dev", "data.analytics.com"
    };

    const std::vector<std::string> paths = {
        "/api/v1/users/", "/data/records/", "/content/posts/",
        "/files/images/", "/cache/objects/", "/search/results/"
    };

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> domain_dist(0, domains.size() - 1);
    std::uniform_int_distribution<> path_dist(0, paths.size() - 1);

    for (size_t i = 0; i < n; i++) {
        std::string url = "https://" + domains[domain_dist(gen)] +
                         paths[path_dist(gen)] + std::to_string(i) +
                         "?id=" + std::to_string(i * 7919);
        keys.push_back(url);
    }

    return keys;
}

std::vector<std::string> generate_random_strings(size_t n, size_t min_len = 10, size_t max_len = 50) {
    // Generate random alphanumeric strings
    std::vector<std::string> keys;
    keys.reserve(n);

    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    const size_t charset_size = sizeof(charset) - 1;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> len_dist(min_len, max_len);
    std::uniform_int_distribution<> char_dist(0, charset_size - 1);

    for (size_t i = 0; i < n; i++) {
        size_t len = len_dist(gen);
        std::string key;
        key.reserve(len);

        for (size_t j = 0; j < len; j++) {
            key += charset[char_dist(gen)];
        }

        keys.push_back(key);
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

    // Positive lookup phase (keys in the dataset)
    auto lookup_start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < std::min(keys.size(), size_t(10000)); i++) {
        volatile uint64_t h = model.hash(keys[i]);
        (void)h;
    }
    auto lookup_end = std::chrono::high_resolution_clock::now();

    double lookup_ns = std::chrono::duration<double, std::nano>(lookup_end - lookup_start).count() /
                       std::min(keys.size(), size_t(10000));

    // Negative lookup phase (keys NOT in the dataset)
    auto negative_keys = generate_negative_keys(std::min(keys.size(), size_t(1000)));
    auto neg_lookup_start = std::chrono::high_resolution_clock::now();
    for (const auto& key : negative_keys) {
        volatile uint64_t h = model.hash(key);
        (void)h;
    }
    auto neg_lookup_end = std::chrono::high_resolution_clock::now();

    double neg_lookup_ns = std::chrono::duration<double, std::nano>(neg_lookup_end - neg_lookup_start).count() /
                           negative_keys.size();

    std::cout << std::fixed << std::setprecision(3);
    std::cout << "  Build time: " << build_ms << " ms\n";
    std::cout << "  Avg lookup (positive): " << lookup_ns << " ns\n";
    std::cout << "  Avg lookup (negative): " << neg_lookup_ns << " ns\n";
    std::cout << "  Memory: " << model.getMemoryUsage() << " bytes\n";
    std::cout << "  Bits/key: " << (model.getMemoryUsage() * 8.0 / keys.size()) << "\n";

    // Display construction statistics if available
    auto stats = model.getConstructionStats();
    if (stats.success && stats.attempts > 0) {
        std::cout << "  Construction success: " << (stats.success ? "Yes" : "No")
                  << " (attempts: " << stats.attempts << ")\n";
    }

    model.printStats();
    std::cout << "\n";
}

void benchmark_model_statistical(HashModel& model, const std::vector<std::string>& keys, size_t num_runs = 10) {
    std::cout << "Model: " << model.getName() << " (Statistical Analysis over " << num_runs << " runs)\n";
    std::cout << std::string(60, '-') << "\n";

    // Build phase (single build)
    auto build_start = std::chrono::high_resolution_clock::now();
    model.build(keys);
    auto build_end = std::chrono::high_resolution_clock::now();
    double build_ms = std::chrono::duration<double, std::milli>(build_end - build_start).count();

    // Multiple runs for lookup statistics
    std::vector<double> lookup_times;
    lookup_times.reserve(num_runs);

    size_t lookup_count = std::min(keys.size(), size_t(1000));

    for (size_t run = 0; run < num_runs; run++) {
        auto lookup_start = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < lookup_count; i++) {
            volatile uint64_t h = model.hash(keys[i]);
            (void)h;
        }
        auto lookup_end = std::chrono::high_resolution_clock::now();

        double run_time_ns = std::chrono::duration<double, std::nano>(lookup_end - lookup_start).count() / lookup_count;
        lookup_times.push_back(run_time_ns);
    }

    // Calculate statistics
    std::sort(lookup_times.begin(), lookup_times.end());

    double mean = std::accumulate(lookup_times.begin(), lookup_times.end(), 0.0) / num_runs;

    double variance = 0.0;
    for (double t : lookup_times) {
        variance += (t - mean) * (t - mean);
    }
    variance /= num_runs;
    double stddev = std::sqrt(variance);

    double median = lookup_times[num_runs / 2];
    double p95 = lookup_times[static_cast<size_t>(num_runs * 0.95)];
    double p99 = lookup_times[static_cast<size_t>(num_runs * 0.99)];
    double min_time = lookup_times.front();
    double max_time = lookup_times.back();

    // Negative lookups
    auto negative_keys = generate_negative_keys(500);
    auto neg_lookup_start = std::chrono::high_resolution_clock::now();
    for (const auto& key : negative_keys) {
        volatile uint64_t h = model.hash(key);
        (void)h;
    }
    auto neg_lookup_end = std::chrono::high_resolution_clock::now();
    double neg_lookup_ns = std::chrono::duration<double, std::nano>(neg_lookup_end - neg_lookup_start).count() / negative_keys.size();

    std::cout << std::fixed << std::setprecision(2);
    std::cout << "  Build time: " << build_ms << " ms\n";
    std::cout << "  Lookup (positive) mean: " << mean << " ns (± " << stddev << " ns)\n";
    std::cout << "  Lookup (positive) median: " << median << " ns\n";
    std::cout << "  Lookup (positive) p95: " << p95 << " ns\n";
    std::cout << "  Lookup (positive) p99: " << p99 << " ns\n";
    std::cout << "  Lookup (positive) range: [" << min_time << ", " << max_time << "] ns\n";
    std::cout << "  Lookup (negative) mean: " << neg_lookup_ns << " ns\n";
    std::cout << "  Memory: " << model.getMemoryUsage() << " bytes\n";
    std::cout << "  Bits/key: " << std::setprecision(3) << (model.getMemoryUsage() * 8.0 / keys.size()) << "\n";

    // Display construction statistics
    auto stats = model.getConstructionStats();
    if (stats.success && stats.attempts > 0) {
        std::cout << "  Construction: " << stats.attempts << " attempt(s), "
                  << (stats.success ? "SUCCESS" : "FAILED") << "\n";
        if (stats.chi_square > 0) {
            std::cout << "  Chi-square: " << std::setprecision(1) << stats.chi_square
                      << " (expected: ~" << (keys.size() - 1) << " for uniform)\n";
        }
    }

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
    
    // Extended Hybrid Models
    print_header("4. EXTENDED HYBRID MODELS");
    
    std::cout << "--- 4.1 Adaptive Security Hybrid ---\n\n";
    
    AdaptiveSecurityHybrid ash_low;
    ash_low.setSecurityLevel(1);
    benchmark_model(ash_low, keys);
    
    AdaptiveSecurityHybrid ash_medium;
    ash_medium.setSecurityLevel(4);
    benchmark_model(ash_medium, keys);
    
    AdaptiveSecurityHybrid ash_high;
    ash_high.setSecurityLevel(7);
    benchmark_model(ash_high, keys);
    
    std::cout << "--- 4.2 Parallel Verification Hybrid ---\n\n";
    
    ParallelVerificationHybrid pvh;
    benchmark_model(pvh, keys);
    
    std::cout << "--- 4.3 Cache-Partitioned Security Hybrid ---\n\n";
    
    CachePartitionedHybrid cpsh;
    benchmark_model(cpsh, keys);
    
    std::cout << "--- 4.4 Ultra-Low-Memory Secure Hybrid ---\n\n";
    
    UltraLowMemoryHybrid ulmsh;
    benchmark_model(ulmsh, keys);
    
    std::cout << "--- 4.5 Two-Path Hybrid (Fast + Secure Lanes) ---\n\n";
    
    TwoPathHybrid tphfs;
    benchmark_model(tphfs, keys);
    
    // Comparison Summary
    print_header("5. COMPARATIVE SUMMARY");
    
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
    
    std::cout << std::setw(35) << "Adaptive Security (Low)"
              << std::setw(15) << "~11-16" << std::setw(15) << "~very-fast"
              << "Tunable security\n";
              
    std::cout << std::setw(35) << "Adaptive Security (High)"
              << std::setw(15) << "~11-16" << std::setw(15) << "~medium"
              << "Full verification\n";
              
    std::cout << std::setw(35) << "Parallel Verification"
              << std::setw(15) << "~12-17" << std::setw(15) << "~fast"
              << "Latency hiding\n";
              
    std::cout << std::setw(35) << "Cache-Partitioned"
              << std::setw(15) << "~13-18" << std::setw(15) << "~fast"
              << "Hardware-aware, Bloom\n";
              
    std::cout << std::setw(35) << "Ultra-Low-Memory"
              << std::setw(15) << "~1.5-2.5" << std::setw(15) << "~slower"
              << "Extreme space\n";
              
    std::cout << std::setw(35) << "Two-Path (Fast+Secure)"
              << std::setw(15) << "~18-24" << std::setw(15) << "~mixed"
              << "Dual lanes\n";

    // Diverse Dataset Testing
    print_header("6. DIVERSE DATASET TESTING");

    std::cout << "Testing with realistic key patterns to validate generalization...\n\n";

    std::cout << "--- 6.1 UUID Keys (1000 keys) ---\n";
    auto uuid_keys = generate_uuid_keys(1000);
    BDZ_MPHF bdz_uuid;
    benchmark_model_statistical(bdz_uuid, uuid_keys, 5);

    std::cout << "--- 6.2 URL Keys (1000 keys) ---\n";
    auto url_keys = generate_url_keys(1000);
    BDZ_MPHF bdz_url;
    benchmark_model_statistical(bdz_url, url_keys, 5);

    std::cout << "--- 6.3 Random Strings (1000 keys) ---\n";
    auto random_keys = generate_random_strings(1000);
    BDZ_MPHF bdz_random;
    benchmark_model_statistical(bdz_random, random_keys, 5);

    // Scalability Testing
    print_header("7. SCALABILITY ANALYSIS");

    std::cout << "Testing BDZ MPHF at different scales...\n\n";

    for (size_t n : {100, 1000, 10000}) {
        std::cout << "--- Dataset size: " << n << " keys ---\n";
        auto scale_keys = generate_test_keys(n);
        BDZ_MPHF bdz_scale;
        benchmark_model_statistical(bdz_scale, scale_keys, 5);
    }

    print_header("EVALUATION COMPLETE");

    return 0;
}

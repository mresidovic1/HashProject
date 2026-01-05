#include <iostream>
#include <iomanip>
#include <vector>
#include <memory>
#include <string>

// Include all hash implementations
#include "basic_perfect_hash.hpp"
#include "basic_crypto_hash.hpp"
#include "bdz_mphf.hpp"
#include "ultimate_hybrid.hpp"
#include "adaptive_security_hybrid.hpp"
#include "parallel_verification_hybrid.hpp"
#include "cache_partitioned_hybrid.hpp"
#include "ultra_low_memory_hybrid.hpp"
#include "two_path_hybrid.hpp"

// Include benchmark suite
#include "benchmark_suite.hpp"

using namespace hashing;
using namespace benchmark;

// Factory function to create all algorithms
std::vector<std::unique_ptr<HashModel>> createAllAlgorithms() {
    std::vector<std::unique_ptr<HashModel>> algorithms;
    
    algorithms.push_back(std::make_unique<BasicPerfectHash>());
    algorithms.push_back(std::make_unique<BasicCryptoHash>());
    algorithms.push_back(std::make_unique<BDZ_MPHF>());
    algorithms.push_back(std::make_unique<UltimateHybrid>());
    algorithms.push_back(std::make_unique<AdaptiveSecurityHybrid>());
    algorithms.push_back(std::make_unique<ParallelVerificationHybrid>());
    algorithms.push_back(std::make_unique<CachePartitionedHybrid>());
    algorithms.push_back(std::make_unique<UltraLowMemoryHybrid>());
    algorithms.push_back(std::make_unique<TwoPathHybrid>());
    
    return algorithms;
}

int main() {
    std::cout << "=================================================================\n";
    std::cout << "  ADVANCED HASH ALGORITHM BENCHMARKING SUITE\n";
    std::cout << "=================================================================\n\n";
    
    // Define test sizes: 1k, 10k, 100k, 1M, 5M, 10M
    std::vector<size_t> test_sizes = {
        1000,           // 1k
        10000,          // 10k
        100000,         // 100k
        1000000,        // 1M
        5000000,        // 5M
        10000000        // 10M
    };
    
    // Define distributions
    std::vector<KeyDistribution> distributions = {
        KeyDistribution::UNIFORM,
        KeyDistribution::GAUSSIAN
    };
    
    BenchmarkSuite suite;
    
    // Run benchmarks for each combination
    for (auto dist : distributions) {
        std::string dist_name = (dist == KeyDistribution::UNIFORM) ? "UNIFORM" : "GAUSSIAN";
        std::cout << "\n--- Testing with " << dist_name << " distribution ---\n";
        
        for (auto size : test_sizes) {
            std::cout << "\nGenerating " << size << " keys...\n";
            auto keys = KeyGenerator::generate(size, dist);
            
            std::cout << "Testing all algorithms with " << size << " keys...\n";
            
            // Create fresh instances for each test
            auto algorithms = createAllAlgorithms();
            
            int algo_count = 0;
            for (auto& algo : algorithms) {
                algo_count++;
                std::cout << "  [" << algo_count << "/9] " << algo->getName() << "... ";
                std::cout.flush();
                
                try {
                    auto result = suite.runBenchmark(algo.get(), keys, dist);
                    suite.addResult(result);
                    
                    std::cout << "✓ Insert: " << std::fixed << std::setprecision(2) 
                              << result.insert_ops_per_sec << " Mops/s, "
                              << "Lookup: " << result.lookup_ops_per_sec << " Mops/s, "
                              << "Memory: " << result.memory_usage_mb << " MB\n";
                              
                } catch (const std::exception& e) {
                    std::cout << "✗ Failed: " << e.what() << "\n";
                    
                    // Add failed result with zeros
                    BenchmarkResult failed_result;
                    failed_result.algorithm_name = algo->getName();
                    failed_result.num_keys = size;
                    failed_result.distribution = dist_name;
                    failed_result.insert_ops_per_sec = 0.0;
                    failed_result.lookup_ops_per_sec = 0.0;
                    failed_result.memory_usage_mb = 0.0;
                    failed_result.build_time_ms = 0.0;
                    suite.addResult(failed_result);
                }
            }
        }
    }
    
    // Export results to CSV
    std::string csv_filename = "benchmark_results.csv";
    std::cout << "\n=================================================================\n";
    std::cout << "Exporting results to: " << csv_filename << "\n";
    std::cout << "=================================================================\n";
    
    try {
        suite.exportToCSV(csv_filename);
        std::cout << "✓ Successfully exported " << suite.getResults().size() << " results!\n";
    } catch (const std::exception& e) {
        std::cerr << "✗ Failed to export CSV: " << e.what() << "\n";
        return 1;
    }
    
    // Print summary statistics
    std::cout << "\n=================================================================\n";
    std::cout << "  SUMMARY\n";
    std::cout << "=================================================================\n";
    std::cout << "Total tests run: " << suite.getResults().size() << "\n";
    std::cout << "Distributions tested: 2 (Uniform, Gaussian)\n";
    std::cout << "Key sizes tested: " << test_sizes.size() << "\n";
    std::cout << "Algorithms tested: 9\n";
    std::cout << "\nResults saved to: " << csv_filename << "\n";
    std::cout << "Ready for Python/Seaborn visualization!\n";
    std::cout << "=================================================================\n";
    
    return 0;
}

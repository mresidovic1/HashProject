#ifndef BENCHMARK_SUITE_HPP
#define BENCHMARK_SUITE_HPP

#include <vector>
#include <string>
#include <chrono>
#include <memory>
#include <fstream>
#include <random>
#include <cstdint>
#include "base_hash.hpp"

namespace benchmark {

// Key distribution types
enum class KeyDistribution {
    UNIFORM,
    GAUSSIAN
};

// Benchmark result structure
struct BenchmarkResult {
    std::string algorithm_name;
    size_t num_keys;
    std::string distribution;
    double insert_ops_per_sec;    // Mops/s
    double lookup_ops_per_sec;    // Mops/s
    double memory_usage_mb;
    double build_time_ms;
};

// Key generator class
class KeyGenerator {
public:
    static std::vector<std::string> generateUniform(size_t n, uint64_t seed = 42) {
        std::vector<std::string> keys;
        keys.reserve(n);
        std::mt19937_64 rng(seed);
        
        for (size_t i = 0; i < n; i++) {
            keys.push_back("key_" + std::to_string(rng()) + "_" + std::to_string(i));
        }
        
        return keys;
    }
    
    static std::vector<std::string> generateGaussian(size_t n, uint64_t seed = 42) {
        std::vector<std::string> keys;
        keys.reserve(n);
        std::mt19937_64 rng(seed);
        std::normal_distribution<double> dist(5000.0, 1000.0); // mean=5000, stddev=1000
        
        for (size_t i = 0; i < n; i++) {
            uint64_t gaussian_value = static_cast<uint64_t>(std::abs(dist(rng)));
            keys.push_back("key_" + std::to_string(gaussian_value) + "_cluster_" + std::to_string(i));
        }
        
        return keys;
    }
    
    static std::vector<std::string> generate(size_t n, KeyDistribution dist, uint64_t seed = 42) {
        switch (dist) {
            case KeyDistribution::UNIFORM:
                return generateUniform(n, seed);
            case KeyDistribution::GAUSSIAN:
                return generateGaussian(n, seed);
            default:
                return generateUniform(n, seed);
        }
    }
};

// Main benchmark suite class
class BenchmarkSuite {
private:
    std::vector<BenchmarkResult> results_;
    
public:
    // Benchmark a single algorithm
    BenchmarkResult runBenchmark(
        hashing::HashModel* model,
        const std::vector<std::string>& keys,
        KeyDistribution dist
    ) {
        BenchmarkResult result;
        result.algorithm_name = model->getName();
        result.num_keys = keys.size();
        result.distribution = (dist == KeyDistribution::UNIFORM) ? "uniform" : "gaussian";
        
        // Measure build time (insert operation)
        auto build_start = std::chrono::high_resolution_clock::now();
        model->build(keys);
        auto build_end = std::chrono::high_resolution_clock::now();
        
        std::chrono::duration<double, std::milli> build_duration = build_end - build_start;
        result.build_time_ms = build_duration.count();
        
        // Calculate insert throughput (Mops/s)
        double build_time_sec = build_duration.count() / 1000.0;
        result.insert_ops_per_sec = (keys.size() / build_time_sec) / 1e6;
        
        // Measure lookup time
        auto lookup_start = std::chrono::high_resolution_clock::now();
        for (const auto& key : keys) {
            volatile uint64_t hash_val = model->hash(key);
            (void)hash_val; // Prevent optimization
        }
        auto lookup_end = std::chrono::high_resolution_clock::now();
        
        std::chrono::duration<double, std::milli> lookup_duration = lookup_end - lookup_start;
        double lookup_time_sec = lookup_duration.count() / 1000.0;
        result.lookup_ops_per_sec = (keys.size() / lookup_time_sec) / 1e6;
        
        // Get memory usage
        size_t memory_bytes = model->getMemoryUsage();
        result.memory_usage_mb = memory_bytes / (1024.0 * 1024.0);
        
        return result;
    }
    
    // Add result to collection
    void addResult(const BenchmarkResult& result) {
        results_.push_back(result);
    }
    
    // Export results to CSV
    void exportToCSV(const std::string& filename) const {
        std::ofstream file(filename);
        
        if (!file.is_open()) {
            throw std::runtime_error("Could not open file: " + filename);
        }
        
        // Write header
        file << "Algorithm,Num_Keys,Distribution,Insert_Ops_Per_Sec,Lookup_Ops_Per_Sec,Memory_Usage_MB,Build_Time_MS\n";
        
        // Write data
        for (const auto& result : results_) {
            file << result.algorithm_name << ","
                 << result.num_keys << ","
                 << result.distribution << ","
                 << std::fixed << std::setprecision(6) << result.insert_ops_per_sec << ","
                 << std::fixed << std::setprecision(6) << result.lookup_ops_per_sec << ","
                 << std::fixed << std::setprecision(3) << result.memory_usage_mb << ","
                 << std::fixed << std::setprecision(2) << result.build_time_ms << "\n";
        }
        
        file.close();
    }
    
    // Clear all results
    void clear() {
        results_.clear();
    }
    
    // Get all results
    const std::vector<BenchmarkResult>& getResults() const {
        return results_;
    }
};

} // namespace benchmark

#endif // BENCHMARK_SUITE_HPP

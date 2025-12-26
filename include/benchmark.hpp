#ifndef BENCHMARK_HPP
#define BENCHMARK_HPP

#include "base_hash.hpp"
#include <chrono>
#include <vector>
#include <string>

namespace hashing {

struct BenchmarkResult {
    std::string model_name;
    double build_time_ms;
    double avg_lookup_time_ns;
    double stddev_lookup_ns;       // Standard deviation of lookup times
    double median_lookup_ns;       // Median lookup time
    double p95_lookup_ns;          // 95th percentile
    double p99_lookup_ns;          // 99th percentile
    double min_lookup_ns;          // Minimum lookup time
    double max_lookup_ns;          // Maximum lookup time
    size_t memory_bytes;
    double memory_per_key_bits;
    size_t num_keys;
    size_t num_runs;               // Number of benchmark runs
};

class Benchmark {
private:
    std::vector<std::string> test_keys;
    std::vector<std::string> query_keys;
    
public:
    void loadKeys(size_t num_keys);
    BenchmarkResult run(HashModel& model);
    void printComparison(const std::vector<BenchmarkResult>& results);
    void printDetailedAnalysis(const std::vector<BenchmarkResult>& results);
};

} // namespace hashing

#endif // BENCHMARK_HPP

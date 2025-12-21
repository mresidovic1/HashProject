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
    size_t memory_bytes;
    double memory_per_key_bits;
    size_t num_keys;
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

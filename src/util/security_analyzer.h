#pragma once
#include <string>
#include <vector>
#include <chrono>
#include <functional>
#include <map>

// Struktura za čuvanje rezultata testova
struct TestResult {
    std::string algorithmName;
    int testSize;
    double executionTimeMs;
    int collisionCount;
    double averageHashTime;
    bool securityPassed;
    std::string additionalInfo;
};

// Struktura za analizu performansi
struct PerformanceMetrics {
    double hashesPerSecond;
    double memoryUsageMB;
    double cpuUtilization;
    double avgTimeMs;
    double medianTimeMs;
    double minTimeMs;
    double maxTimeMs;
    double stdDevMs;
};

// Comprehensive benchmark results
struct BenchmarkResult {
    std::string algorithmName;
    PerformanceMetrics performance;
    TestResult collisionTest;
    double avalancheScore;
    bool timingResistant;
    std::string securityLevel; // "High", "Medium", "Low", "Not Secure"
    std::map<std::string, std::string> additionalMetrics;
};

class SecurityAnalyzer {
public:
    // Testiranje otpornosti na kolizije
    static TestResult testCollisionResistance(
        const std::string& algorithmName,
        std::function<std::string(const std::string&)> hashFunction,
        int testSize
    );
    
    // Testiranje otpornosti na brute-force napade
    static bool testBruteForceResistance(
        std::function<std::string(const std::string&, const std::string&)> hashFunction,
        int complexity
    );
    
    // Testiranje otpornosti na rainbow table napade
    static bool testRainbowTableResistance(
        std::function<std::string(const std::string&, const std::string&)> hashFunction
    );
    
    // Testiranje distribucije hash vrednosti (avalanche effect)
    static double testAvalancheEffect(
        std::function<std::string(const std::string&)> hashFunction,
        int samples
    );
    
    // Testiranje timing napada (constant-time operacije)
    static bool testTimingAttackResistance(
        std::function<bool(const std::string&, const std::string&)> verifyFunction
    );
    
    // Testiranje sa zlonamjernim unosima koji ciljaju slabosti jednostavnih hash funkcija
    static TestResult testCollisionWithAdversarialInputs(
        const std::string& algorithmName,
        std::function<std::string(const std::string&)> hashFunction
    );
    
    // NEW: Comprehensive performance benchmark
    static PerformanceMetrics benchmarkPerformance(
        const std::string& algorithmName,
        std::function<std::string(const std::string&)> hashFunction,
        int iterations = 10000
    );
    
    // NEW: Full security analysis
    static BenchmarkResult comprehensiveAnalysis(
        const std::string& algorithmName,
        std::function<std::string(const std::string&)> hashFunction,
        bool includeTimingTest = false
    );
    
    // NEW: Compare multiple hash functions
    static void compareHashFunctions(
        const std::vector<std::pair<std::string, std::function<std::string(const std::string&)>>>& functions
    );
    
    // NEW: Test preimage resistance (one-way property)
    static bool testPreimageResistance(
        std::function<std::string(const std::string&)> hashFunction,
        int attempts = 10000
    );
    
    // NEW: Distribution uniformity test (Chi-square)
    static double testDistributionUniformity(
        std::function<std::string(const std::string&)> hashFunction,
        int samples = 10000
    );
    
    // Helper: Print benchmark result
    static void printBenchmarkResult(const BenchmarkResult& result);
    
    // Helper: Print comparison table
    static void printComparisonTable(const std::vector<BenchmarkResult>& results);
};

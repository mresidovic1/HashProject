#pragma once
#include <string>
#include <vector>
#include <chrono>
#include <functional>

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
};

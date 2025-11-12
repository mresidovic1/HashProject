#include "security_analyzer.h"
#include "util.h"
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <random>
#include <bitset>
#include <functional>
#include <iostream>
#include <iomanip>
#include <cmath>

TestResult SecurityAnalyzer::testCollisionResistance(
    const std::string& algorithmName,
    std::function<std::string(const std::string&)> hashFunction,
    int testSize) {
    
    TestResult result;
    result.algorithmName = algorithmName;
    result.testSize = testSize;
    result.collisionCount = 0;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    std::unordered_set<std::string> hashes;
    std::vector<std::string> inputs;
    
    // Generisanje različitih ulaznih podataka
    for (int i = 0; i < testSize; ++i) {
        inputs.push_back(generateRandomPassword(16) + std::to_string(i));
    }
    
    // Računanje hash vrednosti i detekcija kolizija
    double totalHashTime = 0.0;
    for (const auto& input : inputs) {
        auto hashStart = std::chrono::high_resolution_clock::now();
        std::string hash = hashFunction(input);
        auto hashEnd = std::chrono::high_resolution_clock::now();
        
        totalHashTime += std::chrono::duration<double, std::milli>(hashEnd - hashStart).count();
        
        if (hashes.find(hash) != hashes.end()) {
            result.collisionCount++;
        }
        hashes.insert(hash);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    result.executionTimeMs = std::chrono::duration<double, std::milli>(end - start).count();
    result.averageHashTime = totalHashTime / testSize;
    result.securityPassed = (result.collisionCount == 0);
    result.additionalInfo = "Unique hashes: " + std::to_string(hashes.size());
    
    return result;
}

bool SecurityAnalyzer::testBruteForceResistance(
    std::function<std::string(const std::string&, const std::string&)> hashFunction,
    int complexity) {
    
    // Simulacija brute-force napada
    std::string targetPassword = "SecureP@ss123";
    std::string salt = generateSalt(16);
    std::string targetHash = hashFunction(targetPassword, salt);
    
    // Pokušaj pronalaženja lozinke sa ograničenim pokušajima
    int attempts = 0;
    int maxAttempts = std::min(complexity, 10000); // Ograničavanje za vreme testiranja
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < maxAttempts; ++i) {
        std::string guess = generateRandomPassword(13);
        std::string guessHash = hashFunction(guess, salt);
        attempts++;
        
        if (guessHash == targetHash && guess == targetPassword) {
            // Našao je tačnu lozinku
            return false; // Nije otporan
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    double elapsed = std::chrono::duration<double>(end - start).count();
    
    // Procena vremena za kompletan brute-force
    // Ako treba dugo vremena za mali broj pokušaja, algoritam je otporan
    return (elapsed / attempts) > 0.0001; // Minimum 0.1ms po pokušaju
}

bool SecurityAnalyzer::testRainbowTableResistance(
    std::function<std::string(const std::string&, const std::string&)> hashFunction) {
    
    // Testiranje sa različitim salt vrednostima
    std::string password = "CommonPassword123";
    std::unordered_set<std::string> hashes;
    
    // Generisanje hash-eva sa različitim salt-ovima
    for (int i = 0; i < 100; ++i) {
        std::string salt = generateSalt(16);
        std::string hash = hashFunction(password, salt);
        hashes.insert(hash);
    }
    
    // Ako su svi hash-evi različiti, otporan je na rainbow table napade
    return hashes.size() == 100;
}

double SecurityAnalyzer::testAvalancheEffect(
    std::function<std::string(const std::string&)> hashFunction,
    int samples) {
    
    double totalBitDifference = 0.0;
    
    for (int i = 0; i < samples; ++i) {
        std::string input1 = generateRandomPassword(32);
        std::string input2 = input1;
        
        // Promeni jedan bit u input2
        if (!input2.empty()) {
            input2[input2.length() / 2] ^= 0x01;
        }
        
        std::string hash1 = hashFunction(input1);
        std::string hash2 = hashFunction(input2);
        
        // Use the utility function for accurate bit difference
        totalBitDifference += bitDifferencePercentage(hash1, hash2);
    }
    
    // Idealan avalanche effect je ~50% (0.5)
    return totalBitDifference / samples;
}

bool SecurityAnalyzer::testTimingAttackResistance(
    std::function<bool(const std::string&, const std::string&)> verifyFunction) {
    
    std::string correctValue = "CorrectHashValue123456789ABCDEF";
    std::string wrongValue1 = "WrongHashValue123456789ABCDEF0";  // Razlika na kraju
    std::string wrongValue2 = "0rongHashValue123456789ABCDEF0";  // Razlika na početku
    
    std::vector<double> times;
    
    // Merenje vremena za različite pogrešne vrednosti
    for (int i = 0; i < 1000; ++i) {
        auto start = std::chrono::high_resolution_clock::now();
        verifyFunction(correctValue, wrongValue1);
        auto end = std::chrono::high_resolution_clock::now();
        times.push_back(std::chrono::duration<double, std::nano>(end - start).count());
    }
    
    double avgTime1 = calculateMean(times);
    times.clear();
    
    for (int i = 0; i < 1000; ++i) {
        auto start = std::chrono::high_resolution_clock::now();
        verifyFunction(correctValue, wrongValue2);
        auto end = std::chrono::high_resolution_clock::now();
        times.push_back(std::chrono::duration<double, std::nano>(end - start).count());
    }
    
    double avgTime2 = calculateMean(times);
    
    // Ako je razlika u vremenu mala (<5%), otporan je na timing napade
    double difference = std::abs(avgTime1 - avgTime2) / std::max(avgTime1, avgTime2);
    return difference < 0.05;
}

TestResult SecurityAnalyzer::testCollisionWithAdversarialInputs(
    const std::string& algorithmName,
    std::function<std::string(const std::string&)> hashFunction) {

    TestResult result;
    result.algorithmName = algorithmName;
    result.collisionCount = 0;

    // Primjeri "zlonamjernih" unosa
    std::vector<std::string> inputs = {
        "Aa", "BB", "costarring", "liquid", "declinate", "macallums", "sialadenitis",
        "zoning", "quintuplets", "caret", "isograms", "desorb", "subgenera",
        "pericarp", "subordination", "teargas", "saltier", "recapitalization",
        "anoint", "monomorphism", "sialadenitises", "subgenerical", "ragouter",
        "Anglo-Indian", "artiness", "detesters", "character", "customising"
    };

    // Proširimo listu da povećamo šanse za koliziju
    int originalSize = inputs.size();
    for (int i = 0; i < originalSize; ++i) {
        std::string s = inputs[i];
        std::reverse(s.begin(), s.end());
        inputs.push_back(s);
        inputs.push_back(inputs[i] + " " + inputs[i]);
    }
    result.testSize = inputs.size();

    auto start = std::chrono::high_resolution_clock::now();
    
    std::unordered_map<std::string, std::string> hashes;
    double totalHashTime = 0.0;

    for (const auto& input : inputs) {
        auto hashStart = std::chrono::high_resolution_clock::now();
        std::string hash = hashFunction(input);
        auto hashEnd = std::chrono::high_resolution_clock::now();
        
        totalHashTime += std::chrono::duration<double, std::milli>(hashEnd - hashStart).count();
        
        if (hashes.find(hash) != hashes.end()) {
            result.collisionCount++;
            result.additionalInfo = "Collision: '" + hashes[hash] + "' vs '" + input + "'";
        } else {
            hashes[hash] = input;
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    result.executionTimeMs = std::chrono::duration<double, std::milli>(end - start).count();
    result.averageHashTime = totalHashTime / result.testSize;
    result.securityPassed = (result.collisionCount == 0);
    
    return result;
}

// NEW METHODS

PerformanceMetrics SecurityAnalyzer::benchmarkPerformance(
    const std::string& algorithmName,
    std::function<std::string(const std::string&)> hashFunction,
    int iterations) {
    
    PerformanceMetrics metrics;
    std::vector<double> times;
    times.reserve(iterations);
    
    // Generate test data
    std::vector<std::string> testData;
    for (int i = 0; i < iterations; ++i) {
        testData.push_back(generateRandomPassword(32));
    }
    
    // Benchmark
    auto startTotal = std::chrono::high_resolution_clock::now();
    
    for (const auto& data : testData) {
        auto start = std::chrono::high_resolution_clock::now();
        hashFunction(data);
        auto end = std::chrono::high_resolution_clock::now();
        times.push_back(std::chrono::duration<double, std::milli>(end - start).count());
    }
    
    auto endTotal = std::chrono::high_resolution_clock::now();
    double totalTime = std::chrono::duration<double>(endTotal - startTotal).count();
    
    metrics.hashesPerSecond = iterations / totalTime;
    metrics.avgTimeMs = calculateMean(times);
    metrics.medianTimeMs = calculateMedian(times);
    metrics.minTimeMs = *std::min_element(times.begin(), times.end());
    metrics.maxTimeMs = *std::max_element(times.begin(), times.end());
    metrics.stdDevMs = calculateStdDev(times);
    metrics.memoryUsageMB = 0.0; // Would need platform-specific code
    metrics.cpuUtilization = 0.0; // Would need platform-specific code
    
    return metrics;
}

BenchmarkResult SecurityAnalyzer::comprehensiveAnalysis(
    const std::string& algorithmName,
    std::function<std::string(const std::string&)> hashFunction,
    bool includeTimingTest) {
    
    BenchmarkResult result;
    result.algorithmName = algorithmName;
    
    std::cout << "Analyzing " << algorithmName << "..." << std::endl;
    
    // Performance benchmark
    result.performance = benchmarkPerformance(algorithmName, hashFunction, 1000);
    
    // Collision resistance
    result.collisionTest = testCollisionResistance(algorithmName, hashFunction, 5000);
    
    // Avalanche effect
    result.avalancheScore = testAvalancheEffect(hashFunction, 100);
    
    // Timing attack resistance (if requested)
    if (includeTimingTest) {
        auto verifyFunc = [&](const std::string& a, const std::string& b) {
            return constantTimeCompare(a, b);
        };
        result.timingResistant = testTimingAttackResistance(verifyFunc);
    } else {
        result.timingResistant = false;
    }
    
    // Determine security level
    if (result.collisionTest.collisionCount == 0 && 
        result.avalancheScore > 0.4 && result.avalancheScore < 0.6) {
        result.securityLevel = "High";
    } else if (result.collisionTest.collisionCount == 0 && result.avalancheScore > 0.3) {
        result.securityLevel = "Medium";
    } else if (result.collisionTest.collisionCount < 10) {
        result.securityLevel = "Low";
    } else {
        result.securityLevel = "Not Secure";
    }
    
    // Additional metrics
    result.additionalMetrics["Hashes/Second"] = std::to_string(static_cast<int>(result.performance.hashesPerSecond));
    result.additionalMetrics["Avalanche %"] = std::to_string(static_cast<int>(result.avalancheScore * 100));
    result.additionalMetrics["Collisions"] = std::to_string(result.collisionTest.collisionCount);
    
    return result;
}

void SecurityAnalyzer::compareHashFunctions(
    const std::vector<std::pair<std::string, std::function<std::string(const std::string&)>>>& functions) {
    
    std::vector<BenchmarkResult> results;
    
    for (const auto& [name, func] : functions) {
        results.push_back(comprehensiveAnalysis(name, func, false));
    }
    
    printComparisonTable(results);
}

bool SecurityAnalyzer::testPreimageResistance(
    std::function<std::string(const std::string&)> hashFunction,
    int attempts) {
    
    // Generate a target hash
    std::string originalInput = generateRandomPassword(32);
    std::string targetHash = hashFunction(originalInput);
    
    // Try to find another input that produces the same hash
    for (int i = 0; i < attempts; ++i) {
        std::string guess = generateRandomPassword(32);
        if (guess != originalInput && hashFunction(guess) == targetHash) {
            return false; // Found preimage!
        }
    }
    
    return true; // No preimage found
}

double SecurityAnalyzer::testDistributionUniformity(
    std::function<std::string(const std::string&)> hashFunction,
    int samples) {
    
    const int NUM_BUCKETS = 256;
    std::vector<int> buckets(NUM_BUCKETS, 0);
    
    // Generate hashes and count first byte distribution
    for (int i = 0; i < samples; ++i) {
        std::string input = generateRandomString(20) + std::to_string(i);
        std::string hash = hashFunction(input);
        
        if (hash.length() >= 2) {
            // Get first byte from hex
            std::string firstByte = hash.substr(0, 2);
            int bucketIndex = std::stoi(firstByte, nullptr, 16);
            buckets[bucketIndex]++;
        }
    }
    
    // Calculate chi-square statistic
    double expected = static_cast<double>(samples) / NUM_BUCKETS;
    double chiSquare = 0.0;
    
    for (int count : buckets) {
        double diff = count - expected;
        chiSquare += (diff * diff) / expected;
    }
    
    // Lower chi-square = better uniformity
    // For 255 degrees of freedom, chi-square around 255 is good
    return chiSquare;
}

void SecurityAnalyzer::printBenchmarkResult(const BenchmarkResult& result) {
    printHeader(result.algorithmName + " Analysis");
    
    std::cout << "Security Level: " << result.securityLevel << std::endl;
    std::cout << "\nPerformance:" << std::endl;
    std::cout << "  Throughput: " << std::fixed << std::setprecision(0) 
              << result.performance.hashesPerSecond << " hashes/sec" << std::endl;
    std::cout << "  Avg Time: " << std::fixed << std::setprecision(4) 
              << result.performance.avgTimeMs << " ms" << std::endl;
    std::cout << "  Median Time: " << std::fixed << std::setprecision(4) 
              << result.performance.medianTimeMs << " ms" << std::endl;
    
    std::cout << "\nSecurity Metrics:" << std::endl;
    std::cout << "  Collisions (5000 tests): " << result.collisionTest.collisionCount << std::endl;
    std::cout << "  Avalanche Effect: " << std::fixed << std::setprecision(1) 
              << (result.avalancheScore * 100) << "% (ideal: 50%)" << std::endl;
    std::cout << "  Security Status: " << (result.collisionTest.securityPassed ? "PASSED" : "FAILED") 
              << std::endl;
    
    printSeparator();
}

void SecurityAnalyzer::printComparisonTable(const std::vector<BenchmarkResult>& results) {
    printHeader("Hash Function Comparison");
    
    std::cout << std::left << std::setw(25) << "Algorithm"
              << std::setw(15) << "Security"
              << std::setw(15) << "Speed (h/s)"
              << std::setw(15) << "Collisions"
              << std::setw(15) << "Avalanche %"
              << std::endl;
    printSeparator('-', 85);
    
    for (const auto& result : results) {
        std::cout << std::left << std::setw(25) << result.algorithmName
                  << std::setw(15) << result.securityLevel
                  << std::setw(15) << std::fixed << std::setprecision(0) 
                  << result.performance.hashesPerSecond
                  << std::setw(15) << result.collisionTest.collisionCount
                  << std::setw(15) << std::fixed << std::setprecision(1) 
                  << (result.avalancheScore * 100)
                  << std::endl;
    }
    
    printSeparator();
}

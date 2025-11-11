#include "security_analyzer.h"
#include "util.h"
#include <unordered_set>
#include <algorithm>
#include <random>
#include <bitset>
#include <functional>

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
        
        // Brojanje različitih bitova
        int differentBits = 0;
        int totalBits = std::min(hash1.length(), hash2.length()) * 4; // Hex = 4 bita
        
        for (size_t j = 0; j < std::min(hash1.length(), hash2.length()); ++j) {
            if (hash1[j] != hash2[j]) {
                // Gruba aproksimacija - svaki različit hex karakter = ~2 bita razlike
                differentBits += 2;
            }
        }
        
        if (totalBits > 0) {
            totalBitDifference += static_cast<double>(differentBits) / totalBits;
        }
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
    
    double avgTime1 = 0.0;
    for (double t : times) avgTime1 += t;
    avgTime1 /= times.size();
    
    times.clear();
    
    for (int i = 0; i < 1000; ++i) {
        auto start = std::chrono::high_resolution_clock::now();
        verifyFunction(correctValue, wrongValue2);
        auto end = std::chrono::high_resolution_clock::now();
        times.push_back(std::chrono::duration<double, std::nano>(end - start).count());
    }
    
    double avgTime2 = 0.0;
    for (double t : times) avgTime2 += t;
    avgTime2 /= times.size();
    
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

    // Primjeri "zlonamjernih" unosa. Mnoge jednostavne hash funkcije
    // (posebno starije implementacije std::hash) su imale problema sa
    // stringovima koji su permutacije jedni drugih ili imaju sličnu strukturu.
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
            result.additionalInfo = "Collision found for hash " + hash + 
                                    " between input '" + hashes[hash] + 
                                    "' and '" + input + "'";
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

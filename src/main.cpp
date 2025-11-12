/**
 * Hash Function Analysis and Security Testing Framework
 * 
 * This application demonstrates and analyzes both perfect and cryptographic
 * hash functions, focusing on:
 * - Security properties (collision resistance, avalanche effect, preimage resistance)
 * - Performance characteristics (throughput, latency, memory usage)
 * - Practical applications (password storage, data integrity, authentication)
 * 
 * Author: Hash Project Team
 * Date: 2024
 */

#include <iostream>
#include <vector>
#include <string>
#include <iomanip>

// Utility headers
#include "util/util.h"
#include "util/security_analyzer.h"

// Hash function headers
#include "hash/sha256.h"
#include "hash/hmac.h"
#include "hash/pbkdf2.h"
#include "hash/argon2_simple.h"
#include "hash/simple_hash.h"
#include "hash/iterative_hash.h"
#include "hash/mphf.h"

// Function prototypes
void showMainMenu();
void demonstrateCryptographicHashes();
void demonstratePasswordHashing();
void demonstratePerfectHashing();
void demonstrateSecurityAnalysis();
void demonstrateBlockchainHashChain();
void demonstrateHMACAuthentication();
void comparativeAnalysis();
void runAllDemonstrations();

int main() {
    printHeader("HASH FUNCTION ANALYSIS AND SECURITY FRAMEWORK");
    std::cout << "A comprehensive implementation and analysis of perfect and cryptographic hash functions\n";
    std::cout << "Focus: Security, Efficiency, and Collision Resistance\n" << std::endl;
    
    int choice;
    bool running = true;
    
    while (running) {
        showMainMenu();
        std::cout << "\nEnter your choice: ";
        std::cin >> choice;
        std::cout << std::endl;
        
        switch (choice) {
            case 1:
                demonstrateCryptographicHashes();
                break;
            case 2:
                demonstratePasswordHashing();
                break;
            case 3:
                demonstratePerfectHashing();
                break;
            case 4:
                demonstrateSecurityAnalysis();
                break;
            case 5:
                demonstrateBlockchainHashChain();
                break;
            case 6:
                demonstrateHMACAuthentication();
                break;
            case 7:
                comparativeAnalysis();
                break;
            case 8:
                runAllDemonstrations();
                break;
            case 0:
                std::cout << "Exiting program. Thank you!" << std::endl;
                running = false;
                break;
            default:
                std::cout << "Invalid choice. Please try again." << std::endl;
        }
        
        if (running) {
            std::cout << "\n\nPress Enter to continue...";
            std::cin.ignore();
            std::cin.get();
            std::cout << "\n\n";
        }
    }
    
    return 0;
}

void showMainMenu() {
    printSeparator('=', 80);
    std::cout << "MAIN MENU\n";
    printSeparator('=', 80);
    std::cout << "1. Cryptographic Hash Functions (SHA-256)\n";
    std::cout << "2. Password Hashing (PBKDF2, Argon2)\n";
    std::cout << "3. Perfect Hash Functions (MPHF, Cuckoo Hashing)\n";
    std::cout << "4. Security Analysis & Testing\n";
    std::cout << "5. Blockchain Hash Chain Demonstration\n";
    std::cout << "6. HMAC Authentication\n";
    std::cout << "7. Comparative Analysis (All Hash Functions)\n";
    std::cout << "8. Run All Demonstrations\n";
    std::cout << "0. Exit\n";
    printSeparator('=', 80);
}

void demonstrateCryptographicHashes() {
    printHeader("CRYPTOGRAPHIC HASH FUNCTIONS");
    
    std::string testData = "Hello, World! This is a test message.";
    std::cout << "Input: " << testData << "\n" << std::endl;
    
    // SHA-256
    std::cout << "SHA-256 Hash:\n";
    std::string sha256Hash = CryptoHash::sha256(testData);
    std::cout << "  " << sha256Hash << std::endl;
    std::cout << "  Length: " << sha256Hash.length() << " characters (32 bytes)\n" << std::endl;
    
    // Demonstrate avalanche effect
    std::cout << "Avalanche Effect Demonstration:\n";
    std::string testData2 = "Hello, World! This is a test message!"; // Added one character
    std::string sha256Hash2 = CryptoHash::sha256(testData2);
    
    std::cout << "  Original: " << testData << "\n";
    std::cout << "  Hash:     " << sha256Hash << "\n\n";
    std::cout << "  Modified: " << testData2 << " (added '!' at end)\n";
    std::cout << "  Hash:     " << sha256Hash2 << "\n\n";
    
    double bitDiff = bitDifferencePercentage(sha256Hash, sha256Hash2);
    std::cout << "  Bit difference: " << std::fixed << std::setprecision(1) 
              << (bitDiff * 100) << "% (ideal: ~50%)\n";
    
    // Hash with salt
    std::cout << "\n\nSalted Hash (prevents rainbow table attacks):\n";
    std::string password = "MyPassword123";
    std::string salt1 = generateSalt(16);
    std::string salt2 = generateSalt(16);
    
    std::cout << "  Password: " << password << "\n";
    std::cout << "  Salt 1:   " << salt1 << "\n";
    std::cout << "  Hash 1:   " << CryptoHash::sha256WithSalt(password, salt1) << "\n\n";
    std::cout << "  Salt 2:   " << salt2 << "\n";
    std::cout << "  Hash 2:   " << CryptoHash::sha256WithSalt(password, salt2) << "\n";
    std::cout << "\n  Note: Same password with different salts produces different hashes\n";
}

void demonstratePasswordHashing() {
    printHeader("PASSWORD HASHING ALGORITHMS");
    
    std::string password = "MySecurePassword!@#123";
    std::cout << "Password: " << password << "\n" << std::endl;
    
    // PBKDF2
    std::cout << "1. PBKDF2 (Password-Based Key Derivation Function 2)\n";
    printSeparator('-', 80);
    
    Timer timer;
    timer.start();
    std::string pbkdf2Hash = CryptoHash::pbkdf2WithSalt(password, 100000); // 100k iterations
    timer.stop();
    
    std::cout << "  Iterations: 100,000\n";
    std::cout << "  Hash: " << pbkdf2Hash << "\n";
    std::cout << "  Time: " << formatTime(timer.elapsedMilliseconds()) << "\n";
    std::cout << "  Verification: " << (CryptoHash::verifyPBKDF2Hash(password, pbkdf2Hash) ? "PASSED" : "FAILED") << "\n";
    
    // Argon2
    std::cout << "\n2. Argon2 (Memory-Hard Password Hashing)\n";
    printSeparator('-', 80);
    
    timer.start();
    std::string argon2Hash = CryptoHash::argon2WithSalt(password, 4096, 2); // 4MB, 2 iterations
    timer.stop();
    
    std::cout << "  Memory: 4 MB\n";
    std::cout << "  Iterations: 2\n";
    std::cout << "  Hash: " << argon2Hash << "\n";
    std::cout << "  Time: " << formatTime(timer.elapsedMilliseconds()) << "\n";
    std::cout << "  Verification: " << (CryptoHash::verifyArgon2Hash(password, argon2Hash) ? "PASSED" : "FAILED") << "\n";
    
    // Comparison with wrong password
    std::cout << "\n3. Wrong Password Test\n";
    printSeparator('-', 80);
    std::string wrongPassword = "WrongPassword123";
    std::cout << "  Trying wrong password: " << wrongPassword << "\n";
    std::cout << "  PBKDF2 Verification: " << (CryptoHash::verifyPBKDF2Hash(wrongPassword, pbkdf2Hash) ? "PASSED" : "FAILED") << "\n";
    std::cout << "  Argon2 Verification: " << (CryptoHash::verifyArgon2Hash(wrongPassword, argon2Hash) ? "PASSED" : "FAILED") << "\n";
    
    std::cout << "\nSecurity Notes:\n";
    std::cout << "  - PBKDF2: Widely supported, NIST approved, resistant to dictionary attacks\n";
    std::cout << "  - Argon2: Winner of Password Hashing Competition, memory-hard (GPU resistant)\n";
    std::cout << "  - Both use salts to prevent rainbow table attacks\n";
    std::cout << "  - Computation time is intentional (prevents brute-force attacks)\n";
}

void demonstratePerfectHashing() {
    printHeader("PERFECT HASH FUNCTIONS");
    
    // Prepare test data - static key set
    std::vector<std::string> keywords = {
        "class", "public", "private", "protected", "virtual",
        "static", "const", "void", "int", "double", "float",
        "string", "return", "if", "else", "for", "while",
        "switch", "case", "break", "continue", "namespace"
    };
    
    std::cout << "Use Case: Compiler keyword lookup (static dictionary)\n";
    std::cout << "Keywords: " << keywords.size() << " programming language keywords\n" << std::endl;
    
    // Build MPHF
    std::cout << "Building Minimal Perfect Hash Function (CHM Algorithm)...\n";
    PerfectHash::MPHF mphf;
    
    Timer timer;
    timer.start();
    bool buildSuccess = mphf.build(keywords);
    timer.stop();
    
    if (buildSuccess) {
        auto stats = mphf.getStats();
        std::cout << "  Status: SUCCESS\n";
        std::cout << "  Build time: " << formatTime(stats.buildTimeMs) << "\n";
        std::cout << "  Attempts: " << stats.attempts << "\n";
        std::cout << "  Memory: " << formatBytes(stats.memoryBytes) << "\n";
        std::cout << "  Bits per key: " << std::fixed << std::setprecision(2) << stats.bitsPerKey << "\n";
    } else {
        std::cout << "  Status: FAILED (could not build acyclic graph)\n";
        return;
    }
    
    // Test lookups
    std::cout << "\nLookup Tests:\n";
    printSeparator('-', 80);
    
    std::vector<std::string> testKeys = {"class", "return", "xyz", "void", "notakeyword"};
    for (const auto& key : testKeys) {
        int index = mphf.lookup(key);
        std::cout << "  lookup(\"" << key << "\") = " << index;
        if (index != -1) {
            std::cout << " (found)\n";
        } else {
            std::cout << " (not found)\n";
        }
    }
    
    // Benchmark lookup performance
    std::cout << "\nPerformance Benchmark:\n";
    printSeparator('-', 80);
    
    const int ITERATIONS = 100000;
    timer.start();
    for (int i = 0; i < ITERATIONS; ++i) {
        mphf.lookup(keywords[i % keywords.size()]);
    }
    timer.stop();
    
    double lookupsPerSecond = ITERATIONS / timer.elapsedSeconds();
    std::cout << "  Lookups: " << ITERATIONS << "\n";
    std::cout << "  Time: " << formatTime(timer.elapsedMilliseconds()) << "\n";
    std::cout << "  Throughput: " << std::fixed << std::setprecision(0) 
              << lookupsPerSecond << " lookups/second\n";
    
    // Cuckoo Hashing demonstration
    std::cout << "\n\nCuckoo Hashing (Dynamic Perfect Hashing):\n";
    printSeparator('-', 80);
    
    PerfectHash::CuckooHash cuckoo(50);
    
    std::cout << "Inserting 20 key-value pairs...\n";
    for (int i = 0; i < 20; ++i) {
        std::string key = "key" + std::to_string(i);
        std::string value = "value" + std::to_string(i);
        cuckoo.insert(key, value);
    }
    
    std::cout << "  Elements: " << cuckoo.size() << "\n";
    std::cout << "  Load factor: " << std::fixed << std::setprecision(2) 
              << cuckoo.loadFactor() << "\n";
    
    std::cout << "\nLookup tests:\n";
    std::string value;
    if (cuckoo.lookup("key5", value)) {
        std::cout << "  key5 -> " << value << "\n";
    }
    if (cuckoo.lookup("key15", value)) {
        std::cout << "  key15 -> " << value << "\n";
    }
    if (!cuckoo.lookup("key999", value)) {
        std::cout << "  key999 -> not found\n";
    }
    
    std::cout << "\nPerfect Hashing Summary:\n";
    std::cout << "  - MPHF: Zero collisions, minimal space, O(1) lookup, static keys only\n";
    std::cout << "  - Cuckoo: O(1) worst-case lookup, supports insertion/deletion\n";
    std::cout << "  - Use cases: Compilers, routers, spell checkers, embedded systems\n";
}

void demonstrateSecurityAnalysis() {
    printHeader("SECURITY ANALYSIS & TESTING");
    
    std::cout << "Running comprehensive security tests on hash functions...\n" << std::endl;
    
    // Test simple hash (should show weaknesses)
    std::cout << "1. Testing Simple Hash (DJB2) - Educational Example\n";
    printSeparator('-', 80);
    auto simpleHashFunc = [](const std::string& input) { return SimpleHash::djb2Hash(input); };
    auto result1 = SecurityAnalyzer::comprehensiveAnalysis("DJB2 Simple Hash", simpleHashFunc, false);
    SecurityAnalyzer::printBenchmarkResult(result1);
    
    std::cout << "\n\n2. Testing SHA-256 - Cryptographic Standard\n";
    printSeparator('-', 80);
    auto sha256Func = [](const std::string& input) { return CryptoHash::sha256(input); };
    auto result2 = SecurityAnalyzer::comprehensiveAnalysis("SHA-256", sha256Func, false);
    SecurityAnalyzer::printBenchmarkResult(result2);
    
    // Adversarial input test
    std::cout << "\n\n3. Adversarial Input Collision Test\n";
    printSeparator('-', 80);
    std::cout << "Testing with known problematic inputs for simple hash functions...\n\n";
    
    auto advTest1 = SecurityAnalyzer::testCollisionWithAdversarialInputs("DJB2", simpleHashFunc);
    std::cout << "DJB2 Collisions: " << advTest1.collisionCount << " / " << advTest1.testSize << "\n";
    if (advTest1.collisionCount > 0) {
        std::cout << "  " << advTest1.additionalInfo << "\n";
    }
    
    auto advTest2 = SecurityAnalyzer::testCollisionWithAdversarialInputs("SHA-256", sha256Func);
    std::cout << "SHA-256 Collisions: " << advTest2.collisionCount << " / " << advTest2.testSize << "\n";
}

void demonstrateBlockchainHashChain() {
    printHeader("BLOCKCHAIN HASH CHAIN DEMONSTRATION");
    
    std::cout << "Simulating a simple blockchain with hash chaining...\n" << std::endl;
    
    struct Block {
        int index;
        std::string data;
        std::string previousHash;
        std::string hash;
        std::string timestamp;
    };
    
    std::vector<Block> blockchain;
    
    // Genesis block
    Block genesis;
    genesis.index = 0;
    genesis.data = "Genesis Block";
    genesis.previousHash = "0000000000000000000000000000000000000000000000000000000000000000";
    genesis.timestamp = getCurrentTimestamp();
    genesis.hash = CryptoHash::sha256(std::to_string(genesis.index) + genesis.data + 
                                      genesis.previousHash + genesis.timestamp);
    blockchain.push_back(genesis);
    
    // Add more blocks
    std::vector<std::string> transactions = {
        "Alice sends 10 BTC to Bob",
        "Bob sends 5 BTC to Charlie",
        "Charlie sends 3 BTC to Alice",
        "Alice sends 2 BTC to Dave"
    };
    
    for (size_t i = 0; i < transactions.size(); ++i) {
        Block block;
        block.index = blockchain.size();
        block.data = transactions[i];
        block.previousHash = blockchain.back().hash;
        block.timestamp = getCurrentTimestamp();
        block.hash = CryptoHash::sha256(std::to_string(block.index) + block.data + 
                                        block.previousHash + block.timestamp);
        blockchain.push_back(block);
    }
    
    // Display blockchain
    for (const auto& block : blockchain) {
        std::cout << "Block #" << block.index << "\n";
        std::cout << "  Data: " << block.data << "\n";
        std::cout << "  Timestamp: " << block.timestamp << "\n";
        std::cout << "  Previous Hash: " << block.previousHash.substr(0, 16) << "...\n";
        std::cout << "  Hash: " << block.hash.substr(0, 16) << "...\n" << std::endl;
    }
    
    // Demonstrate tampering detection
    std::cout << "\nTampering Detection:\n";
    printSeparator('-', 80);
    std::cout << "Attempting to modify Block #2...\n";
    
    Block tamperedBlock = blockchain[2];
    tamperedBlock.data = "Charlie sends 100 BTC to Alice (TAMPERED!)";
    tamperedBlock.hash = CryptoHash::sha256(std::to_string(tamperedBlock.index) + 
                                            tamperedBlock.data + 
                                            tamperedBlock.previousHash + 
                                            tamperedBlock.timestamp);
    
    std::cout << "  Original: " << blockchain[2].data << "\n";
    std::cout << "  Tampered: " << tamperedBlock.data << "\n\n";
    
    // Check if block 3's previous hash matches
    bool chainValid = (blockchain[3].previousHash == tamperedBlock.hash);
    std::cout << "  Block #3 previous hash matches tampered block: " 
              << (chainValid ? "YES" : "NO") << "\n";
    std::cout << "  Chain integrity: " << (chainValid ? "COMPROMISED" : "INTACT") << "\n";
    std::cout << "\n  This demonstrates how hash chaining ensures data integrity!\n";
}

void demonstrateHMACAuthentication() {
    printHeader("HMAC AUTHENTICATION");
    
    std::cout << "HMAC (Hash-based Message Authentication Code)\n";
    std::cout << "Used for: API authentication, message integrity, JWT signing\n" << std::endl;
    
    std::string secretKey = generateSalt(32);
    std::string message = "Transfer $1000 from Account A to Account B";
    
    std::cout << "Secret Key: " << secretKey << "\n";
    std::cout << "Message: " << message << "\n" << std::endl;
    
    // Generate HMAC
    std::string hmac = CryptoHash::hmacSHA256(message, secretKey);
    std::cout << "HMAC-SHA256: " << hmac << "\n" << std::endl;
    
    // Verify authentic message
    std::cout << "Verification Tests:\n";
    printSeparator('-', 80);
    
    bool valid1 = CryptoHash::verifyHMAC(message, secretKey, hmac);
    std::cout << "1. Authentic message: " << (valid1 ? "VALID" : "INVALID") << "\n";
    
    // Try with wrong key
    std::string wrongKey = generateSalt(32);
    bool valid2 = CryptoHash::verifyHMAC(message, wrongKey, hmac);
    std::cout << "2. Wrong key: " << (valid2 ? "VALID" : "INVALID") << "\n";
    
    // Try with modified message
    std::string tamperedMessage = "Transfer $9999 from Account A to Account B";
    bool valid3 = CryptoHash::verifyHMAC(tamperedMessage, secretKey, hmac);
    std::cout << "3. Tampered message: " << (valid3 ? "VALID" : "INVALID") << "\n";
    
    std::cout << "\nUse Cases:\n";
    std::cout << "  - API Authentication (AWS Signature, OAuth)\n";
    std::cout << "  - JWT (JSON Web Tokens)\n";
    std::cout << "  - Webhook signatures\n";
    std::cout << "  - Message integrity in secure communications\n";
}

void comparativeAnalysis() {
    printHeader("COMPARATIVE ANALYSIS: ALL HASH FUNCTIONS");
    
    std::cout << "Comparing cryptographic and non-cryptographic hash functions...\n" << std::endl;
    
    std::vector<std::pair<std::string, std::function<std::string(const std::string&)>>> functions = {
        {"SHA-256", [](const std::string& s) { return CryptoHash::sha256(s); }},
        {"DJB2", [](const std::string& s) { return SimpleHash::djb2Hash(s); }},
        {"FNV-1a", [](const std::string& s) { return SimpleHash::fnv1aHash(s); }},
        {"Additive (Bad)", [](const std::string& s) { return SimpleHash::additiveHash(s); }},
        {"Iterative SHA-256", [](const std::string& s) { return IterativeHash::iterativeSHA256(s, 10); }}
    };
    
    SecurityAnalyzer::compareHashFunctions(functions);
    
    std::cout << "\nKey Findings:\n";
    std::cout << "  - Cryptographic hashes (SHA-256): High security, good avalanche effect, slower\n";
    std::cout << "  - Simple hashes (DJB2, FNV-1a): Fast but vulnerable to collisions\n";
    std::cout << "  - Additive hash: Very fast but many collisions (not recommended)\n";
    std::cout << "  - Iterative hashing: Increases time but not memory-hard\n";
}

void runAllDemonstrations() {
    std::cout << "\n\nRunning all demonstrations...\n\n";
    
    demonstrateCryptographicHashes();
    std::cout << "\n\nPress Enter to continue to next demonstration...";
    std::cin.ignore();
    std::cin.get();
    
    demonstratePasswordHashing();
    std::cout << "\n\nPress Enter to continue...";
    std::cin.get();
    
    demonstratePerfectHashing();
    std::cout << "\n\nPress Enter to continue...";
    std::cin.get();
    
    demonstrateBlockchainHashChain();
    std::cout << "\n\nPress Enter to continue...";
    std::cin.get();
    
    demonstrateHMACAuthentication();
    std::cout << "\n\nPress Enter to continue...";
    std::cin.get();
    
    comparativeAnalysis();
}

// main.cpp
#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <chrono>
#include <random>
#include <cmath>
#include <iomanip>

#include "../src/hash/iterative_hash.h"
#include "../src/util/util.h"
#include "../src/mphf/mphf.h"

// CONFIG: podesive vrijednosti
constexpr size_t NUM_PASSWORDS = 10000;   // koliko ukupno unosa želimo testirati
constexpr int PASSWORD_LEN = 8;            // dužina za generirane lozinke
constexpr int ITERATIONS = 200;            // broj iteracija za iterativeHash (smanjeno radi brzine)
constexpr int TRUNCATE_BITS = 32;          // koliko bita simple hash "zadržavamo" (32 je realna alternativa)

std::string make_variant(const std::string &base, int idx){
    // napravi determinističnu varijantu: base + "_" + hex(idx) + small leet
    std::string out = base;
    out += "_v";
    out += std::to_string(idx);
    if(idx % 3 == 0) out += "!";
    if(idx % 5 == 0) out += "#";
    return out;
}

int main(){
    // 1) read base passwords from file
    std::ifstream file("../data/passwords.txt");
    std::vector<std::string> basePasswords;
    if(file.is_open()){
        std::string line;
        while(std::getline(file,line)){
            if(!line.empty()) basePasswords.push_back(line);
        }
        file.close();
    } else {
        std::cerr << "Upozorenje: nije moguce otvoriti ../data/passwords.txt, generisat cu random lozinke.\n";
    }

    // 2) fill passwords vector up to NUM_PASSWORDS by creating deterministic variants
    std::vector<std::string> passwords;
    passwords.reserve(NUM_PASSWORDS);
    std::mt19937 rng((uint32_t)std::chrono::high_resolution_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<int> pick(0, std::max(0,(int)basePasswords.size()-1));

    // if we have some base passwords, use them as seeds for variants
    if(!basePasswords.empty()){
        size_t i = 0;
        while(passwords.size() < NUM_PASSWORDS){
            const std::string &seed = basePasswords[i % basePasswords.size()];
            passwords.push_back(make_variant(seed, (int)i));
            ++i;
        }
    } else {
        // if no base file, generate random passwords
        for(size_t i=0;i<NUM_PASSWORDS;i++){
            passwords.push_back(generateRandomPassword(PASSWORD_LEN));
        }
    }

    std::cout << "Total passwords for test: " << passwords.size() << std::endl;

    // 3) Prepare pepper (global) and containers
    std::string globalPepper = generatePepper(16);

    // Containers for outputs
    std::vector<std::string> shaHashes;
    shaHashes.reserve(passwords.size());

    // For simple hash we simulate "practical" reduced space by truncating to TRUNCATE_BITS
    std::unordered_set<uint64_t> simpleSet; simpleSet.reserve(passwords.size()*2);

    // Timing SHA256 iterative
    auto t0 = std::chrono::high_resolution_clock::now();
    for(const auto &pw : passwords){
        std::string salt = generateSalt(8);
        std::string input = pw + salt + globalPepper;
        std::string sh = iterativeHash(input, ITERATIONS); // SHA256 hexdigest
        shaHashes.push_back(sh);
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    double sha_ms = std::chrono::duration_cast<std::chrono::milliseconds>(t1-t0).count();

    // Timing simple hash (truncated)
    auto t2 = std::chrono::high_resolution_clock::now();
    for(const auto &pw : passwords){
        std::string salt = generateSalt(8);
        std::string input = pw + salt + globalPepper;
        size_t full = std::hash<std::string>{}(input);
        // truncate to TRUNCATE_BITS (take low bits)
        uint64_t truncated = full & ((TRUNCATE_BITS >= 64) ? ~0ULL : ((1ULL<<TRUNCATE_BITS)-1ULL));
        simpleSet.insert(truncated);
    }
    auto t3 = std::chrono::high_resolution_clock::now();
    double simple_ms = std::chrono::duration_cast<std::chrono::milliseconds>(t3-t2).count();

    // 4) compute collisions
    size_t sha_unique = std::unordered_set<std::string>(shaHashes.begin(), shaHashes.end()).size();
    size_t simple_unique = simpleSet.size();

    std::cout << std::fixed << std::setprecision(0);
    std::cout << "[Iterative SHA256] Hashiranje " << passwords.size() 
              << " lozinki trajalo je " << sha_ms << " ms" << std::endl;
    std::cout << "Uniq SHA256 outputs: " << sha_unique << " -> Kolizije SHA256: " 
              << (passwords.size() - sha_unique) << std::endl;

    std::cout << "[Simple(std::hash) truncated to " << TRUNCATE_BITS << " bits] Hashiranje " 
              << passwords.size() << " lozinki trajalo je " << simple_ms << " ms" << std::endl;
    std::cout << "Uniq truncated simple outputs: " << simple_unique << " -> Kolizije simple: " 
              << (passwords.size() - simple_unique) << std::endl;

    // 5) basic histogram of first 4 bytes of SHA256 (for distribution)
    std::unordered_map<std::string,int> prefixCount;
    for(const auto &h : shaHashes){
        prefixCount[h.substr(0,8)]++;
    }
    std::cout << "Some SHA256 prefix samples (first 4 bytes hex):" << std::endl;
    int shown = 0;
    for(const auto &p : prefixCount){
        std::cout << p.first << ": " << p.second << "  ";
        if(++shown >= 10) break;
    }
    std::cout << std::endl;

    // 6) Show theoretical approx probability (birthday) of at least one collision for simple truncated space
    double m = double(1ULL << std::min(63, TRUNCATE_BITS)); // careful if TRUNCATE_BITS==64
    double n = double(passwords.size());
    // approximate: p ≈ 1 - exp(- n*(n-1) / (2*m))
    double approx_p = 0.0;
    if(m > 0.0){
        double exponent = - (n*(n-1.0)) / (2.0 * m);
        approx_p = 1.0 - std::exp(exponent);
    }
    std::cout << "Theoretical probability of >=1 collision in truncated space (approx): " 
              << std::setprecision(3) << (approx_p * 100.0) << " %" << std::endl;

    // 7) MPHF build on SHA256 (demonstracija)
    MPHF mphf;
    std::cout << "Building MPHF for SHA256 outputs (this may take a moment)..." << std::endl;
    mphf.build(shaHashes);
    std::string testHash = shaHashes[0];
    int idx = mphf.lookup(testHash);
    std::cout << "MPHF lookup test: " << (idx != -1 ? "found at index " + std::to_string(idx) : "not found") << std::endl;

    return 0;
}

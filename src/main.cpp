#include <iostream>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <chrono>
#include "util/util.h"
#include "hash/iterative_hash.h"
#include "mphf/mphf.h"

struct User {
    std::string username;
    std::string salt;
    std::string pepper;
    std::string password;
    std::string hashedPassword;
};

int main(){
    const int NUM_USERS = 1000;
    const int PASSWORD_LENGTH = 6;
    std::vector<User> users;

    // Generiranje random korisnika i lozinki
    std::string globalPepper = generatePepper(16);
    for(int i=0;i<NUM_USERS;i++){
        User u;
        u.username = "user" + std::to_string(i);
        u.password = generateRandomPassword(PASSWORD_LENGTH);
        u.salt = generateSalt();
        u.pepper = globalPepper;
        users.push_back(u);
    }

    // Iterativni SHA256 hash (sigurno i sporije)
    std::vector<std::string> shaHashes;
    auto startSHA = std::chrono::high_resolution_clock::now();
    for(auto &u: users){
        std::string input = u.password + u.salt + u.pepper;
        u.hashedPassword = iterativeHash(input, 1000);
        shaHashes.push_back(u.hashedPassword);
    }
    auto endSHA = std::chrono::high_resolution_clock::now();
    double durationSHA = std::chrono::duration_cast<std::chrono::milliseconds>(endSHA - startSHA).count();

    // Simple hash (brzo, ali sa kolizijama)
    std::unordered_set<size_t> simpleSet;
    auto startSimple = std::chrono::high_resolution_clock::now();
    for(auto &u: users){
        std::string input = u.password + u.salt + u.pepper;
        size_t h = std::hash<std::string>{}(input) % 1024; // forsiranje kolizija
        simpleSet.insert(h);
    }
    auto endSimple = std::chrono::high_resolution_clock::now();
    double durationSimple = std::chrono::duration_cast<std::chrono::milliseconds>(endSimple - startSimple).count();

    // Prikaz rezultata
    std::cout << "[Iterative SHA256] Hashiranje " << NUM_USERS 
              << " lozinki trajalo je " << durationSHA << " ms" << std::endl;
    std::cout << "Kolizije SHA256: " << NUM_USERS - shaHashes.size() << std::endl;

    std::cout << "[Simple std::hash] Hashiranje " << NUM_USERS 
              << " lozinki trajalo je " << durationSimple << " ms" << std::endl;
    std::cout << "Kolizije simple hash: " << NUM_USERS - simpleSet.size() << std::endl;

    // Histogram prvih 4 bajta SHA256
    std::unordered_map<std::string,int> prefixCount;
    for(auto &h: shaHashes){
        prefixCount[h.substr(0,8)]++;
    }
    std::cout << "Histogram prvih 4 bajta SHA256:" << std::endl;
    for(auto &p: prefixCount){
        std::cout << p.first << ": " << p.second << std::endl;
    }

    // MPHF demonstracija
    MPHF mphf;
    mphf.build(shaHashes);
    std::string testHash = shaHashes[42]; // proizvoljan test
    int idx = mphf.lookup(testHash);
    std::cout << "MPHF lookup test: ";
    if(idx!=-1) std::cout << "Hash found at index " << idx 
                          << " (user=" << users[idx].username << ")" << std::endl;
    else std::cout << "Hash not found!" << std::endl;

    // Demonstracija multiple MPHF lookupa
    int foundCount = 0;
    for(int i=0;i<10;i++){
        std::string th = shaHashes[i*10];
        if(mphf.lookup(th)!=-1) foundCount++;
    }
    std::cout << "MPHF bulk lookup test: " << foundCount << " / 10 hashes found." << std::endl;

    return 0;
}

#include "iterative_hash.h"
#include <functional>

std::string iterativeHash(const std::string &input, int iterations){
    std::string h = input;
    SHA256 sha;
    for(int i=0;i<iterations;i++){
        sha = SHA256();
        sha.update(h);
        h = sha.hexdigest();
    }
    return h;
}

std::string simpleHash(const std::string &input){
    std::hash<std::string> hasher;
    return std::to_string(hasher(input));
}

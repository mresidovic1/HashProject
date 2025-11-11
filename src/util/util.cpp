#include "util.h"
#include <random>
#include <chrono>

std::string generateSalt(int length){
    static const char alphanum[] = 
        "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz!@#$%^&*()_+";
    std::string salt;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, sizeof(alphanum)-2);
    for(int i=0;i<length;i++)
        salt += alphanum[dis(gen)];
    return salt;
}

std::string generatePepper(int length){
    return generateSalt(length);
}

std::string generateRandomPassword(int length){
    return generateSalt(length);
}

uint64_t currentTimeMillis(){
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
}

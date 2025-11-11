#include "simple_hash.h"
#include <functional>
#include <string>

std::string simpleHash(const std::string &input){
    std::hash<std::string> hasher;
    return std::to_string(hasher(input));
}

#pragma once
#include <string>
#include <vector>
#include <cstdint>

class SHA256 {
public:
    SHA256();
    void update(const std::string &data);
    std::string hexdigest();

private:
    void transform(const uint8_t* chunk);
    uint32_t h[8];
    std::vector<uint8_t> buffer;
    uint64_t bitlen;
};

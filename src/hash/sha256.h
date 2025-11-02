#pragma once
#include <cstdint>
#include <vector>
#include <string>

class SHA256 {
private:
    uint32_t h[8];
    uint64_t bitlen;
    std::vector<uint8_t> buffer;

    void transform(const uint8_t* chunk);
    void finalize();

public:
    SHA256();
    void update(const std::string &data);
    std::string hexdigest();
};

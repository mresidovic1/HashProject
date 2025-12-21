#ifndef BLAKE3_HASH_HPP
#define BLAKE3_HASH_HPP

#include "base_hash.hpp"

namespace hashing {

// BLAKE3-inspired cryptographic hash (simplified high-performance variant)
class BLAKE3Hash : public HashModel {
private:
    static const uint32_t IV[8];
    static constexpr size_t CHUNK_LEN = 1024;
    static constexpr size_t BLOCK_LEN = 64;
    
    struct ChunkState {
        uint32_t cv[8];
        uint64_t chunk_counter;
        uint8_t buf[BLOCK_LEN];
        uint8_t buf_len;
        uint8_t blocks_compressed;
    };
    
    void compress_block(uint32_t cv[8], const uint8_t block[BLOCK_LEN], 
                       uint64_t counter, uint32_t flags) const;
    void hash_data(const uint8_t* input, size_t len, uint8_t output[32]) const;
    
public:
    std::string getName() const override { return "BLAKE3-Inspired Hash"; }
    void build(const std::vector<std::string>& keys) override;
    uint64_t hash(const std::string& key) const override;
    size_t getMemoryUsage() const override;
    void printStats() const override;
};

} // namespace hashing

#endif // BLAKE3_HASH_HPP

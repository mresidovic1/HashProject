#ifndef MURMUR_HASH_HPP
#define MURMUR_HASH_HPP

#include <cstdint>
#include <cstring>
#include <string>

namespace hashing {

// MurmurHash3 implementation for 64-bit hash values
// Based on MurmurHash3 by Austin Appleby
// This is a high-quality, fast hash function with good distribution properties
class MurmurHash3 {
public:
    // Compute 64-bit hash with a given seed
    static uint64_t hash64(const std::string& key, uint64_t seed = 0) {
        const uint64_t m = 0xc6a4a7935bd1e995ULL;
        const int r = 47;

        uint64_t h = seed ^ (key.length() * m);

        const uint64_t* data = reinterpret_cast<const uint64_t*>(key.data());
        const uint64_t* end = data + (key.length() / 8);

        while (data != end) {
            uint64_t k;
            // Handle unaligned reads
            std::memcpy(&k, data, sizeof(uint64_t));
            data++;

            k *= m;
            k ^= k >> r;
            k *= m;

            h ^= k;
            h *= m;
        }

        const unsigned char* data2 = reinterpret_cast<const unsigned char*>(data);

        switch (key.length() & 7) {
            case 7: h ^= uint64_t(data2[6]) << 48; [[fallthrough]];
            case 6: h ^= uint64_t(data2[5]) << 40; [[fallthrough]];
            case 5: h ^= uint64_t(data2[4]) << 32; [[fallthrough]];
            case 4: h ^= uint64_t(data2[3]) << 24; [[fallthrough]];
            case 3: h ^= uint64_t(data2[2]) << 16; [[fallthrough]];
            case 2: h ^= uint64_t(data2[1]) << 8;  [[fallthrough]];
            case 1: h ^= uint64_t(data2[0]);
                    h *= m;
        };

        h ^= h >> r;
        h *= m;
        h ^= h >> r;

        return h;
    }

    // Generate three independent hash values for use in BDZ MPHF
    static void hash_triple(const std::string& key, uint64_t seed1, uint64_t seed2, uint64_t seed3,
                           size_t& h0, size_t& h1, size_t& h2, size_t table_size) {
        // Use different seeds to ensure independence
        h0 = hash64(key, seed1) % table_size;
        h1 = hash64(key, seed2) % table_size;
        h2 = hash64(key, seed3) % table_size;

        // Ensure uniqueness (collision resolution)
        if (h1 == h0) h1 = (h1 + 1) % table_size;
        if (h2 == h0 || h2 == h1) h2 = (h2 + 1) % table_size;
    }
};

} // namespace hashing

#endif // MURMUR_HASH_HPP

#ifndef BASE_HASH_HPP
#define BASE_HASH_HPP

#include <vector>
#include <string>
#include <cstdint>

namespace hashing {

// Abstract base class for all hash models
class HashModel {
public:
    virtual ~HashModel() = default;
    virtual std::string getName() const = 0;
    virtual void build(const std::vector<std::string>& keys) = 0;
    virtual uint64_t hash(const std::string& key) const = 0;
    virtual size_t getMemoryUsage() const = 0;
    virtual void printStats() const = 0;
};

} // namespace hashing

#endif // BASE_HASH_HPP

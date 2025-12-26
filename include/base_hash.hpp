#ifndef BASE_HASH_HPP
#define BASE_HASH_HPP

#include <vector>
#include <string>
#include <cstdint>

namespace hashing {

// Construction statistics for tracking hash quality
struct ConstructionStats {
    size_t attempts = 0;           // Number of construction attempts
    size_t collisions = 0;         // Collisions encountered during construction
    double chi_square = 0.0;       // Chi-square statistic for distribution quality
    bool success = false;          // Whether construction succeeded

    ConstructionStats() = default;
};

// Abstract base class for all hash models
class HashModel {
public:
    virtual ~HashModel() = default;
    virtual std::string getName() const = 0;
    virtual void build(const std::vector<std::string>& keys) = 0;
    virtual uint64_t hash(const std::string& key) const = 0;
    virtual size_t getMemoryUsage() const = 0;
    virtual void printStats() const = 0;

    // Get construction statistics (if available)
    virtual ConstructionStats getConstructionStats() const {
        return ConstructionStats();  // Default implementation returns empty stats
    }
};

} // namespace hashing

#endif // BASE_HASH_HPP

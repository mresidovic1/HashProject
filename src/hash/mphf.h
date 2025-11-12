#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>

/**
 * Minimal Perfect Hash Function (MPHF)
 * 
 * A perfect hash function maps n keys to n unique hash values (no collisions).
 * A minimal perfect hash function additionally uses a hash table of size exactly n.
 * 
 * This implementation uses the CHM (Czech, Havas, Majewski) algorithm,
 * which is one of the most practical MPHF algorithms.
 * 
 * Algorithm Overview:
 * 1. Build a 3-graph from the keys using 3 hash functions
 * 2. Check if the graph is acyclic
 * 3. Assign values to vertices to create perfect hash
 * 4. Lookup uses the 3 hash functions and vertex values
 * 
 * Properties:
 * - O(n) expected construction time
 * - O(1) lookup time
 * - Space: ~2.09 bits per key (theoretical minimum is ~1.44 bits)
 * - No collisions for the static key set
 * 
 * Use Cases:
 * - Static dictionaries (e.g., programming language keywords)
 * - Database indexing for read-heavy workloads
 * - Embedded systems with limited memory
 * - Spell checkers, compilers
 * - Router lookup tables
 * 
 * Limitations:
 * - Only works for static key sets (keys must be known in advance)
 * - Adding new keys requires complete rebuild
 * - Construction can fail (requires retry with different seeds)
 */

namespace PerfectHash {

class MPHF {
public:
    MPHF();
    
    /**
     * Build the minimal perfect hash function for given keys
     * @param keys Vector of keys to hash
     * @return True if successful, false if construction failed
     */
    bool build(const std::vector<std::string>& keys);
    
    /**
     * Lookup a key and return its index
     * @param key Key to lookup
     * @return Index (0 to n-1) if key exists, -1 otherwise
     */
    int lookup(const std::string& key) const;
    
    /**
     * Check if a key exists in the hash function
     * @param key Key to check
     * @return True if key exists
     */
    bool contains(const std::string& key) const;
    
    /**
     * Get the number of keys
     */
    size_t size() const { return keys.size(); }
    
    /**
     * Get memory usage in bytes
     */
    size_t memoryUsage() const;
    
    /**
     * Get statistics about the construction
     */
    struct Stats {
        int attempts;           // Number of construction attempts
        double buildTimeMs;     // Build time in milliseconds
        size_t memoryBytes;     // Memory usage in bytes
        double bitsPerKey;      // Bits per key ratio
    };
    
    Stats getStats() const { return stats; }
    
private:
    // Hash function helpers
    uint32_t hash1(const std::string& key) const;
    uint32_t hash2(const std::string& key) const;
    uint32_t hash3(const std::string& key) const;
    
    // Graph construction for CHM algorithm
    struct Edge {
        uint32_t v1, v2, v3;  // Three vertices
        size_t keyIndex;       // Original key index
    };
    
    bool buildGraph(const std::vector<std::string>& inputKeys, 
                    std::vector<Edge>& edges);
    bool isAcyclic(const std::vector<Edge>& edges);
    void assignValues(const std::vector<Edge>& edges);
    
    // Data members
    std::vector<std::string> keys;      // Original keys for verification
    std::vector<uint32_t> g;            // Vertex assignment values
    uint32_t seed1, seed2, seed3;       // Hash function seeds
    size_t tableSize;                   // Size of g array
    Stats stats;
};

/**
 * Cuckoo Hashing (Alternative Perfect Hash Approach)
 * 
 * Cuckoo hashing uses two hash functions and two tables.
 * When inserting, if a position is occupied, the existing item
 * is "kicked out" to its alternative position.
 * 
 * Properties:
 * - O(1) worst-case lookup time
 * - O(1) amortized insertion time
 * - Supports dynamic insertions and deletions
 * - Typically requires load factor < 0.5 for good performance
 */
class CuckooHash {
public:
    CuckooHash(size_t capacity = 1000);
    
    bool insert(const std::string& key, const std::string& value);
    bool remove(const std::string& key);
    bool lookup(const std::string& key, std::string& value) const;
    bool contains(const std::string& key) const;
    
    size_t size() const { return numElements; }
    double loadFactor() const { return static_cast<double>(numElements) / tableSize; }
    
private:
    struct Entry {
        std::string key;
        std::string value;
        bool occupied;
        
        Entry() : occupied(false) {}
    };
    
    uint32_t hash1(const std::string& key) const;
    uint32_t hash2(const std::string& key) const;
    
    bool insertInternal(const std::string& key, const std::string& value, int depth);
    void rehash();
    
    std::vector<Entry> table1;
    std::vector<Entry> table2;
    size_t tableSize;
    size_t numElements;
    uint32_t seed1, seed2;
    static const int MAX_KICKS = 500;
};

} // namespace PerfectHash

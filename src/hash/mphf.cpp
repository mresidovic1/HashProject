#include "mphf.h"
#include "../util/util.h"
#include <algorithm>
#include <queue>
#include <unordered_set>
#include <random>

namespace PerfectHash {

// ============================================================================
// MPHF Implementation (CHM Algorithm)
// ============================================================================

MPHF::MPHF() : seed1(0), seed2(0), seed3(0), tableSize(0) {
    stats.attempts = 0;
    stats.buildTimeMs = 0;
    stats.memoryBytes = 0;
    stats.bitsPerKey = 0;
}

uint32_t MPHF::hash1(const std::string& key) const {
    uint32_t hash = seed1;
    for (char c : key) {
        hash = hash * 31 + static_cast<uint8_t>(c);
    }
    return hash % tableSize;
}

uint32_t MPHF::hash2(const std::string& key) const {
    uint32_t hash = seed2;
    for (char c : key) {
        hash = hash * 37 + static_cast<uint8_t>(c);
    }
    return hash % tableSize;
}

uint32_t MPHF::hash3(const std::string& key) const {
    uint32_t hash = seed3;
    for (char c : key) {
        hash = hash * 41 + static_cast<uint8_t>(c);
    }
    return hash % tableSize;
}

bool MPHF::buildGraph(const std::vector<std::string>& inputKeys, 
                      std::vector<Edge>& edges) {
    edges.clear();
    edges.reserve(inputKeys.size());
    
    std::unordered_set<uint64_t> edgeSet;
    
    for (size_t i = 0; i < inputKeys.size(); ++i) {
        Edge e;
        e.v1 = hash1(inputKeys[i]);
        e.v2 = hash2(inputKeys[i]);
        e.v3 = hash3(inputKeys[i]);
        e.keyIndex = i;
        
        // Check for duplicate edges (would cause collision)
        uint64_t edgeHash = (static_cast<uint64_t>(e.v1) << 32) | 
                            (static_cast<uint64_t>(e.v2) << 16) | 
                            static_cast<uint64_t>(e.v3);
        
        if (edgeSet.count(edgeHash)) {
            return false; // Collision in graph construction
        }
        edgeSet.insert(edgeHash);
        
        edges.push_back(e);
    }
    
    return true;
}

bool MPHF::isAcyclic(const std::vector<Edge>& edges) {
    // Build adjacency list
    std::vector<std::vector<size_t>> adj(tableSize);
    std::vector<int> degree(tableSize, 0);
    
    for (size_t i = 0; i < edges.size(); ++i) {
        adj[edges[i].v1].push_back(i);
        adj[edges[i].v2].push_back(i);
        adj[edges[i].v3].push_back(i);
        degree[edges[i].v1]++;
        degree[edges[i].v2]++;
        degree[edges[i].v3]++;
    }
    
    // Use topological sort to check for cycles
    std::queue<uint32_t> q;
    for (uint32_t i = 0; i < tableSize; ++i) {
        if (degree[i] == 0) {
            q.push(i);
        }
    }
    
    int visited = 0;
    while (!q.empty()) {
        uint32_t v = q.front();
        q.pop();
        visited++;
        
        for (size_t edgeIdx : adj[v]) {
            const Edge& e = edges[edgeIdx];
            if (--degree[e.v1] == 0) q.push(e.v1);
            if (--degree[e.v2] == 0) q.push(e.v2);
            if (--degree[e.v3] == 0) q.push(e.v3);
        }
    }
    
    return visited == static_cast<int>(tableSize);
}

void MPHF::assignValues(const std::vector<Edge>& edges) {
    g.assign(tableSize, 0);
    std::vector<bool> assigned(tableSize, false);
    
    // Build adjacency list
    std::vector<std::vector<size_t>> adj(tableSize);
    std::vector<int> degree(tableSize, 0);
    
    for (size_t i = 0; i < edges.size(); ++i) {
        adj[edges[i].v1].push_back(i);
        adj[edges[i].v2].push_back(i);
        adj[edges[i].v3].push_back(i);
        degree[edges[i].v1]++;
        degree[edges[i].v2]++;
        degree[edges[i].v3]++;
    }
    
    // Start with vertices of degree 1 (leaves)
    std::queue<uint32_t> q;
    for (uint32_t i = 0; i < tableSize; ++i) {
        if (degree[i] == 1) {
            q.push(i);
        }
    }
    
    while (!q.empty()) {
        uint32_t v = q.front();
        q.pop();
        
        if (assigned[v]) continue;
        
        // Find the edge containing this vertex
        for (size_t edgeIdx : adj[v]) {
            const Edge& e = edges[edgeIdx];
            
            // Assign value to make this edge map to correct index
            uint32_t sum = 0;
            int unassignedCount = 0;
            uint32_t unassignedVertex = v;
            
            if (assigned[e.v1]) sum += g[e.v1];
            else { unassignedCount++; unassignedVertex = e.v1; }
            
            if (assigned[e.v2]) sum += g[e.v2];
            else { unassignedCount++; unassignedVertex = e.v2; }
            
            if (assigned[e.v3]) sum += g[e.v3];
            else { unassignedCount++; unassignedVertex = e.v3; }
            
            if (unassignedCount == 1) {
                g[unassignedVertex] = (e.keyIndex - sum + keys.size()) % keys.size();
                assigned[unassignedVertex] = true;
                
                // Update degrees
                if (--degree[e.v1] == 1 && !assigned[e.v1]) q.push(e.v1);
                if (--degree[e.v2] == 1 && !assigned[e.v2]) q.push(e.v2);
                if (--degree[e.v3] == 1 && !assigned[e.v3]) q.push(e.v3);
                
                break;
            }
        }
    }
}

bool MPHF::build(const std::vector<std::string>& inputKeys) {
    if (inputKeys.empty()) return false;
    
    Timer timer;
    timer.start();
    
    keys = inputKeys;
    tableSize = keys.size() * 1.23; // Use 23% more space for better success rate
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint32_t> dist(1, 1000000);
    
    const int MAX_ATTEMPTS = 100;
    bool success = false;
    
    for (int attempt = 0; attempt < MAX_ATTEMPTS && !success; ++attempt) {
        stats.attempts = attempt + 1;
        
        // Generate new seeds
        seed1 = dist(gen);
        seed2 = dist(gen);
        seed3 = dist(gen);
        
        // Build graph
        std::vector<Edge> edges;
        if (!buildGraph(keys, edges)) {
            continue;
        }
        
        // Check if acyclic
        if (!isAcyclic(edges)) {
            continue;
        }
        
        // Assign values
        assignValues(edges);
        
        success = true;
    }
    
    timer.stop();
    stats.buildTimeMs = timer.elapsedMilliseconds();
    stats.memoryBytes = memoryUsage();
    stats.bitsPerKey = (stats.memoryBytes * 8.0) / keys.size();
    
    return success;
}

int MPHF::lookup(const std::string& key) const {
    if (g.empty()) return -1;
    
    uint32_t h1 = hash1(key);
    uint32_t h2 = hash2(key);
    uint32_t h3 = hash3(key);
    
    uint32_t index = (g[h1] + g[h2] + g[h3]) % keys.size();
    
    // Verify it's actually the right key
    if (index < keys.size() && keys[index] == key) {
        return static_cast<int>(index);
    }
    
    return -1;
}

bool MPHF::contains(const std::string& key) const {
    return lookup(key) != -1;
}

size_t MPHF::memoryUsage() const {
    size_t total = 0;
    
    // g array
    total += g.size() * sizeof(uint32_t);
    
    // keys array
    for (const auto& key : keys) {
        total += key.capacity();
    }
    total += keys.capacity() * sizeof(std::string);
    
    // Other data
    total += sizeof(MPHF);
    
    return total;
}

// ============================================================================
// Cuckoo Hash Implementation
// ============================================================================

CuckooHash::CuckooHash(size_t capacity) 
    : tableSize(capacity), numElements(0) {
    table1.resize(tableSize);
    table2.resize(tableSize);
    
    std::random_device rd;
    seed1 = rd();
    seed2 = rd();
}

uint32_t CuckooHash::hash1(const std::string& key) const {
    uint32_t hash = seed1;
    for (char c : key) {
        hash = hash * 31 + static_cast<uint8_t>(c);
    }
    return hash % tableSize;
}

uint32_t CuckooHash::hash2(const std::string& key) const {
    uint32_t hash = seed2;
    for (char c : key) {
        hash = hash * 37 + static_cast<uint8_t>(c);
    }
    return hash % tableSize;
}

bool CuckooHash::insert(const std::string& key, const std::string& value) {
    // Check if key already exists
    std::string dummy;
    if (lookup(key, dummy)) {
        return false;
    }
    
    // Check load factor
    if (loadFactor() > 0.5) {
        rehash();
    }
    
    return insertInternal(key, value, 0);
}

bool CuckooHash::insertInternal(const std::string& key, const std::string& value, int depth) {
    if (depth > MAX_KICKS) {
        rehash();
        return insertInternal(key, value, 0);
    }
    
    uint32_t pos1 = hash1(key);
    
    // Try table 1
    if (!table1[pos1].occupied) {
        table1[pos1].key = key;
        table1[pos1].value = value;
        table1[pos1].occupied = true;
        numElements++;
        return true;
    }
    
    // Try table 2
    uint32_t pos2 = hash2(key);
    if (!table2[pos2].occupied) {
        table2[pos2].key = key;
        table2[pos2].value = value;
        table2[pos2].occupied = true;
        numElements++;
        return true;
    }
    
    // Kick out from table 1
    std::string oldKey = table1[pos1].key;
    std::string oldValue = table1[pos1].value;
    
    table1[pos1].key = key;
    table1[pos1].value = value;
    
    return insertInternal(oldKey, oldValue, depth + 1);
}

bool CuckooHash::remove(const std::string& key) {
    uint32_t pos1 = hash1(key);
    if (table1[pos1].occupied && table1[pos1].key == key) {
        table1[pos1].occupied = false;
        numElements--;
        return true;
    }
    
    uint32_t pos2 = hash2(key);
    if (table2[pos2].occupied && table2[pos2].key == key) {
        table2[pos2].occupied = false;
        numElements--;
        return true;
    }
    
    return false;
}

bool CuckooHash::lookup(const std::string& key, std::string& value) const {
    uint32_t pos1 = hash1(key);
    if (table1[pos1].occupied && table1[pos1].key == key) {
        value = table1[pos1].value;
        return true;
    }
    
    uint32_t pos2 = hash2(key);
    if (table2[pos2].occupied && table2[pos2].key == key) {
        value = table2[pos2].value;
        return true;
    }
    
    return false;
}

bool CuckooHash::contains(const std::string& key) const {
    std::string dummy;
    return lookup(key, dummy);
}

void CuckooHash::rehash() {
    size_t oldSize = tableSize;
    tableSize *= 2;
    
    std::vector<Entry> oldTable1 = std::move(table1);
    std::vector<Entry> oldTable2 = std::move(table2);
    
    table1.clear();
    table2.clear();
    table1.resize(tableSize);
    table2.resize(tableSize);
    numElements = 0;
    
    // Reinsert all elements
    for (const auto& entry : oldTable1) {
        if (entry.occupied) {
            insert(entry.key, entry.value);
        }
    }
    
    for (const auto& entry : oldTable2) {
        if (entry.occupied) {
            insert(entry.key, entry.value);
        }
    }
}

} // namespace PerfectHash

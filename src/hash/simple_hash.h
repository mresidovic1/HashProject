#pragma once
#include <string>

/**
 * Simple Hash Functions (Educational - NOT for Security)
 * 
 * These implementations demonstrate basic hashing concepts but have
 * significant weaknesses. They are included for educational purposes
 * to compare with cryptographic hash functions.
 * 
 * Weaknesses:
 * - High collision probability
 * - Predictable patterns
 * - Not resistant to attacks
 * - Poor avalanche effect
 * 
 * DO NOT USE FOR:
 * - Password storage
 * - Digital signatures
 * - Security-critical applications
 */

namespace SimpleHash {

/**
 * Basic additive hash (sums ASCII values)
 * Worst collision resistance, included for demonstration
 */
std::string additiveHash(const std::string& input);

/**
 * DJB2 hash - better distribution than additive
 * Classic hash function, but not cryptographically secure
 */
std::string djb2Hash(const std::string& input);

/**
 * FNV-1a hash - Fast Non-cryptographic hash
 * Good for hash tables, not for security
 */
std::string fnv1aHash(const std::string& input);

/**
 * Simple polynomial rolling hash
 * Used in Rabin-Karp string matching
 */
std::string polynomialHash(const std::string& input);

/**
 * MurmurHash3-inspired simple version
 * Good distribution for hash tables
 */
std::string murmurHash3Simple(const std::string& input);

} // namespace SimpleHash

#pragma once
#include <string>

/**
 * Argon2 - Memory-Hard Password Hashing
 * 
 * Argon2 is the winner of the Password Hashing Competition (2015).
 * It's specifically designed to resist GPU/ASIC attacks by requiring
 * significant memory, making parallel attacks expensive.
 * 
 * Variants:
 * - Argon2d: Resistant to GPU attacks (data-dependent)
 * - Argon2i: Resistant to side-channel attacks (data-independent)
 * - Argon2id: Hybrid - resistant to both (recommended)
 * 
 * Security Properties:
 * - Memory-hard (configurable memory cost)
 * - Configurable time cost (iterations)
 * - Parallelism support
 * - Resistant to time-memory tradeoff attacks
 * 
 * Use Cases:
 * - Password storage (recommended for new systems)
 * - Key derivation for encryption
 * - Cryptocurrency mining (some variants)
 * 
 * Recommended Parameters (2024):
 * - Memory: 64 MB+ (65536 KB)
 * - Iterations: 3+
 * - Parallelism: 4 threads
 * - Salt: 16+ bytes
 * - Output: 32+ bytes
 * 
 * Note: This is a simplified implementation for educational purposes.
 * For production use, consider using libargon2 library.
 */

namespace CryptoHash {

/**
 * Simplified Argon2-like password hashing
 * Uses PBKDF2 with increased memory operations as approximation
 * 
 * @param password Password to hash
 * @param salt Salt value
 * @param memoryCostKB Memory cost in KB (simulated)
 * @param timeCost Time cost (iterations)
 * @return Hexadecimal hash
 */
std::string argon2Simple(const std::string& password,
                         const std::string& salt,
                         int memoryCostKB = 65536,  // 64 MB
                         int timeCost = 3);

/**
 * Verify password against Argon2-like hash
 * @param password Password to verify
 * @param salt Salt used
 * @param hash Expected hash
 * @param memoryCostKB Memory cost
 * @param timeCost Time cost
 * @return True if password matches
 */
bool verifyArgon2Simple(const std::string& password,
                        const std::string& salt,
                        const std::string& hash,
                        int memoryCostKB = 65536,
                        int timeCost = 3);

/**
 * Generate complete Argon2-like hash with metadata
 * Format: argon2$memory$time$salt$hash
 * @param password Password to hash
 * @param memoryCostKB Memory cost
 * @param timeCost Time cost
 * @return Complete hash string
 */
std::string argon2WithSalt(const std::string& password,
                           int memoryCostKB = 65536,
                           int timeCost = 3);

/**
 * Verify password against complete Argon2 hash string
 * @param password Password to verify
 * @param hashString Complete hash string
 * @return True if password matches
 */
bool verifyArgon2Hash(const std::string& password, const std::string& hashString);

} // namespace CryptoHash

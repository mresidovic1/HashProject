#pragma once
#include <string>

/**
 * PBKDF2 (Password-Based Key Derivation Function 2)
 * 
 * PBKDF2 is designed specifically for password hashing and key derivation.
 * It applies a pseudorandom function (like HMAC-SHA256) many times to
 * increase computational cost, making brute-force attacks impractical.
 * 
 * Security Properties:
 * - Configurable iteration count (work factor)
 * - Requires salt to prevent rainbow table attacks
 * - Standardized (RFC 2898, PKCS #5)
 * - Time-tested and widely adopted
 * 
 * Use Cases:
 * - Password storage in databases
 * - Deriving encryption keys from passwords
 * - WPA2 wireless security
 * - File encryption (e.g., encrypted ZIP files)
 * 
 * Recommended Parameters (2024):
 * - Iterations: 600,000+ for HMAC-SHA256 (OWASP recommendation)
 * - Salt length: 16+ bytes
 * - Output length: 32+ bytes
 */

namespace CryptoHash {

/**
 * Compute PBKDF2-HMAC-SHA256
 * @param password Password to hash
 * @param salt Salt value (should be random and unique per password)
 * @param iterations Number of iterations (higher = more secure but slower)
 * @param outputLength Desired output length in bytes
 * @return Hexadecimal hash
 */
std::string pbkdf2(const std::string& password, 
                   const std::string& salt,
                   int iterations = 600000,
                   int outputLength = 32);

/**
 * Verify password against PBKDF2 hash
 * @param password Password to verify
 * @param salt Salt used in original hash
 * @param hash Expected hash
 * @param iterations Iteration count used
 * @param outputLength Output length used
 * @return True if password matches
 */
bool verifyPBKDF2(const std::string& password,
                  const std::string& salt,
                  const std::string& hash,
                  int iterations = 600000,
                  int outputLength = 32);

/**
 * Generate complete PBKDF2 hash with automatic salt generation
 * Returns format: iterations$salt$hash
 * @param password Password to hash
 * @param iterations Number of iterations
 * @return Complete hash string with metadata
 */
std::string pbkdf2WithSalt(const std::string& password, int iterations = 600000);

/**
 * Verify password against complete PBKDF2 hash string
 * @param password Password to verify
 * @param hashString Complete hash string (iterations$salt$hash)
 * @return True if password matches
 */
bool verifyPBKDF2Hash(const std::string& password, const std::string& hashString);

} // namespace CryptoHash

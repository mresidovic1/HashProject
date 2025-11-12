#pragma once
#include <string>
#include <vector>
#include <cstdint>

/**
 * SHA-256 Hash Function Wrapper
 * 
 * SHA-256 (Secure Hash Algorithm 256-bit) is part of the SHA-2 family.
 * 
 * Security Properties:
 * - Output size: 256 bits (32 bytes)
 * - Collision resistance: ~2^128 operations
 * - Preimage resistance: ~2^256 operations
 * - Second preimage resistance: ~2^256 operations
 * 
 * Use Cases:
 * - Digital signatures
 * - Certificate generation
 * - Blockchain (Bitcoin uses SHA-256)
 * - File integrity verification
 * - Password storage (with proper salt/pepper and key derivation)
 */

namespace CryptoHash {

/**
 * Compute SHA-256 hash of input string
 * @param input Input string to hash
 * @return Hexadecimal string representation of hash (64 characters)
 */
std::string sha256(const std::string& input);

/**
 * Compute SHA-256 hash of byte array
 * @param data Pointer to data
 * @param length Length of data
 * @return Hexadecimal string representation of hash
 */
std::string sha256(const uint8_t* data, size_t length);

/**
 * Compute SHA-256 hash of byte vector
 * @param data Byte vector
 * @return Hexadecimal string representation of hash
 */
std::string sha256(const std::vector<uint8_t>& data);

/**
 * Compute SHA-256 with salt (for password hashing)
 * Note: For production password hashing, use PBKDF2 or Argon2 instead
 * @param password Password to hash
 * @param salt Salt value
 * @return Hexadecimal hash
 */
std::string sha256WithSalt(const std::string& password, const std::string& salt);

} // namespace CryptoHash

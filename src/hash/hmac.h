#pragma once
#include <string>
#include <vector>
#include <cstdint>

/**
 * HMAC (Hash-based Message Authentication Code)
 * 
 * HMAC provides both data integrity and authentication.
 * It combines a cryptographic hash function with a secret key.
 * 
 * Security Properties:
 * - Requires secret key known to both sender and receiver
 * - Prevents tampering and forgery
 * - Resistant to length extension attacks
 * 
 * Use Cases:
 * - API authentication (e.g., AWS Signature Version 4)
 * - JWT (JSON Web Tokens) signing
 * - Message integrity verification
 * - Challenge-response authentication
 */

namespace CryptoHash {

/**
 * Compute HMAC-SHA256
 * @param message Message to authenticate
 * @param key Secret key
 * @return Hexadecimal HMAC
 */
std::string hmacSHA256(const std::string& message, const std::string& key);

/**
 * Compute HMAC-SHA256 with byte arrays
 * @param message Message data
 * @param messageLen Message length
 * @param key Key data
 * @param keyLen Key length
 * @return Hexadecimal HMAC
 */
std::string hmacSHA256(const uint8_t* message, size_t messageLen, 
                       const uint8_t* key, size_t keyLen);

/**
 * Verify HMAC (constant-time comparison to prevent timing attacks)
 * @param message Original message
 * @param key Secret key
 * @param expectedHmac Expected HMAC value
 * @return True if HMAC matches
 */
bool verifyHMAC(const std::string& message, const std::string& key, 
                const std::string& expectedHmac);

} // namespace CryptoHash

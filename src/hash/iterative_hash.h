#pragma once
#include <string>

/**
 * Iterative Hash Function
 * 
 * Demonstrates iterative hashing where a hash function is applied
 * multiple times in sequence. This increases computation time but
 * doesn't provide the same security benefits as proper key derivation
 * functions (KDF) like PBKDF2 or Argon2.
 * 
 * Educational Purpose:
 * - Shows why simple iteration isn't enough for password security
 * - Demonstrates the difference between iterations and memory-hardness
 * - Compares with proper KDFs
 * 
 * Limitations:
 * - Not memory-hard (vulnerable to GPU/ASIC attacks)
 * - No standardized format
 * - Slower than dedicated KDFs with equivalent security
 */

namespace IterativeHash {

/**
 * Apply SHA-256 iteratively
 * @param input Input string
 * @param iterations Number of iterations
 * @return Final hash after iterations
 */
std::string iterativeSHA256(const std::string& input, int iterations);

/**
 * Iterative hash with salt
 * @param input Input string
 * @param salt Salt value
 * @param iterations Number of iterations
 * @return Final hash
 */
std::string iterativeSHA256WithSalt(const std::string& input, 
                                    const std::string& salt,
                                    int iterations);

} // namespace IterativeHash

// For backward compatibility with existing code
std::string iterativeHash(const std::string& input, int iterations);
std::string simpleHash(const std::string& input);

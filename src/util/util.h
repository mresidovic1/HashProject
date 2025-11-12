#pragma once
#include <string>
#include <vector>
#include <random>
#include <chrono>

/**
 * Utility Functions for Hash Project
 * Provides essential helper functions for:
 * - Random data generation (salts, peppers, passwords)
 * - Timing and performance measurement
 * - String manipulation and encoding
 */

// ============================================================================
// RANDOM DATA GENERATION
// ============================================================================

/**
 * Generate cryptographically secure random salt
 * @param length Number of bytes to generate (default: 16)
 * @return Hexadecimal string representation of salt
 */
std::string generateSalt(int length = 16);

/**
 * Generate application-wide pepper (stored securely, not in database)
 * @param length Number of bytes to generate
 * @return Hexadecimal string representation of pepper
 */
std::string generatePepper(int length = 32);

/**
 * Generate random password for testing purposes
 * @param length Length of password
 * @return Random password string with mixed characters
 */
std::string generateRandomPassword(int length = 16);

/**
 * Generate random alphanumeric string
 * @param length Length of string
 * @return Random alphanumeric string
 */
std::string generateRandomString(int length);

/**
 * Generate random bytes
 * @param length Number of bytes
 * @return Vector of random bytes
 */
std::vector<uint8_t> generateRandomBytes(int length);

// ============================================================================
// STRING MANIPULATION AND ENCODING
// ============================================================================

/**
 * Convert bytes to hexadecimal string
 * @param data Byte vector
 * @return Hex string
 */
std::string bytesToHex(const std::vector<uint8_t>& data);

/**
 * Convert bytes to hexadecimal string (from pointer)
 * @param data Pointer to byte array
 * @param length Length of array
 * @return Hex string
 */
std::string bytesToHex(const uint8_t* data, size_t length);

/**
 * Convert hexadecimal string to bytes
 * @param hex Hexadecimal string
 * @return Byte vector
 */
std::vector<uint8_t> hexToBytes(const std::string& hex);

/**
 * Convert string to bytes
 * @param str Input string
 * @return Byte vector
 */
std::vector<uint8_t> stringToBytes(const std::string& str);

/**
 * XOR two byte arrays (useful for crypto operations)
 * @param a First byte array
 * @param b Second byte array
 * @return XOR result
 */
std::vector<uint8_t> xorBytes(const std::vector<uint8_t>& a, const std::vector<uint8_t>& b);

// ============================================================================
// TIMING AND PERFORMANCE
// ============================================================================

/**
 * High-resolution timer for performance measurement
 */
class Timer {
public:
    Timer();
    void start();
    void stop();
    double elapsedMilliseconds() const;
    double elapsedMicroseconds() const;
    double elapsedSeconds() const;
    
private:
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point endTime;
    bool running;
};

// ============================================================================
// COMPARISON AND VALIDATION
// ============================================================================

/**
 * Constant-time string comparison (prevents timing attacks)
 * @param a First string
 * @param b Second string
 * @return True if strings are equal
 */
bool constantTimeCompare(const std::string& a, const std::string& b);

/**
 * Constant-time byte array comparison
 * @param a First array
 * @param b Second array
 * @return True if arrays are equal
 */
bool constantTimeCompare(const std::vector<uint8_t>& a, const std::vector<uint8_t>& b);

/**
 * Calculate Hamming distance between two strings
 * @param a First string
 * @param b Second string
 * @return Number of differing positions
 */
int hammingDistance(const std::string& a, const std::string& b);

/**
 * Calculate bit difference percentage between two hex strings
 * @param hash1 First hash (hex)
 * @param hash2 Second hash (hex)
 * @return Percentage of different bits (0.0 - 1.0)
 */
double bitDifferencePercentage(const std::string& hash1, const std::string& hash2);

// ============================================================================
// STATISTICS AND ANALYSIS
// ============================================================================

/**
 * Calculate mean of a vector of doubles
 */
double calculateMean(const std::vector<double>& values);

/**
 * Calculate standard deviation
 */
double calculateStdDev(const std::vector<double>& values);

/**
 * Calculate median
 */
double calculateMedian(std::vector<double> values);

/**
 * Format time duration for human-readable output
 * @param milliseconds Time in milliseconds
 * @return Formatted string
 */
std::string formatTime(double milliseconds);

/**
 * Format bytes for human-readable output
 * @param bytes Number of bytes
 * @return Formatted string (e.g., "1.5 MB")
 */
std::string formatBytes(size_t bytes);

// ============================================================================
// HELPER FUNCTIONS
// ============================================================================

/**
 * Print separator line for console output
 */
void printSeparator(char c = '=', int length = 80);

/**
 * Print header with title
 */
void printHeader(const std::string& title);

/**
 * Get current timestamp as string
 */
std::string getCurrentTimestamp();

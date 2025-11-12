#include "util.h"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <iostream>
#include <ctime>

// ============================================================================
// RANDOM DATA GENERATION
// ============================================================================

static std::random_device rd;
static std::mt19937 gen(rd());

std::string generateSalt(int length) {
    std::uniform_int_distribution<> dist(0, 255);
    std::vector<uint8_t> salt(length);
    
    for (int i = 0; i < length; ++i) {
        salt[i] = static_cast<uint8_t>(dist(gen));
    }
    
    return bytesToHex(salt);
}

std::string generatePepper(int length) {
    return generateSalt(length); // Same implementation, different semantic purpose
}

std::string generateRandomPassword(int length) {
    const std::string chars = 
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "0123456789"
        "!@#$%^&*()_+-=[]{}|;:,.<>?";
    
    std::uniform_int_distribution<> dist(0, chars.length() - 1);
    std::string password;
    password.reserve(length);
    
    for (int i = 0; i < length; ++i) {
        password += chars[dist(gen)];
    }
    
    return password;
}

std::string generateRandomString(int length) {
    const std::string chars = 
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "0123456789";
    
    std::uniform_int_distribution<> dist(0, chars.length() - 1);
    std::string str;
    str.reserve(length);
    
    for (int i = 0; i < length; ++i) {
        str += chars[dist(gen)];
    }
    
    return str;
}

std::vector<uint8_t> generateRandomBytes(int length) {
    std::uniform_int_distribution<> dist(0, 255);
    std::vector<uint8_t> bytes(length);
    
    for (int i = 0; i < length; ++i) {
        bytes[i] = static_cast<uint8_t>(dist(gen));
    }
    
    return bytes;
}

// ============================================================================
// STRING MANIPULATION AND ENCODING
// ============================================================================

std::string bytesToHex(const std::vector<uint8_t>& data) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    
    for (uint8_t byte : data) {
        ss << std::setw(2) << static_cast<int>(byte);
    }
    
    return ss.str();
}

std::string bytesToHex(const uint8_t* data, size_t length) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    
    for (size_t i = 0; i < length; ++i) {
        ss << std::setw(2) << static_cast<int>(data[i]);
    }
    
    return ss.str();
}

std::vector<uint8_t> hexToBytes(const std::string& hex) {
    std::vector<uint8_t> bytes;
    
    for (size_t i = 0; i < hex.length(); i += 2) {
        std::string byteString = hex.substr(i, 2);
        uint8_t byte = static_cast<uint8_t>(std::stoi(byteString, nullptr, 16));
        bytes.push_back(byte);
    }
    
    return bytes;
}

std::vector<uint8_t> stringToBytes(const std::string& str) {
    return std::vector<uint8_t>(str.begin(), str.end());
}

std::vector<uint8_t> xorBytes(const std::vector<uint8_t>& a, const std::vector<uint8_t>& b) {
    size_t minLen = std::min(a.size(), b.size());
    std::vector<uint8_t> result(minLen);
    
    for (size_t i = 0; i < minLen; ++i) {
        result[i] = a[i] ^ b[i];
    }
    
    return result;
}

// ============================================================================
// TIMING AND PERFORMANCE
// ============================================================================

Timer::Timer() : running(false) {}

void Timer::start() {
    startTime = std::chrono::high_resolution_clock::now();
    running = true;
}

void Timer::stop() {
    endTime = std::chrono::high_resolution_clock::now();
    running = false;
}

double Timer::elapsedMilliseconds() const {
    auto end = running ? std::chrono::high_resolution_clock::now() : endTime;
    return std::chrono::duration<double, std::milli>(end - startTime).count();
}

double Timer::elapsedMicroseconds() const {
    auto end = running ? std::chrono::high_resolution_clock::now() : endTime;
    return std::chrono::duration<double, std::micro>(end - startTime).count();
}

double Timer::elapsedSeconds() const {
    auto end = running ? std::chrono::high_resolution_clock::now() : endTime;
    return std::chrono::duration<double>(end - startTime).count();
}

// ============================================================================
// COMPARISON AND VALIDATION
// ============================================================================

bool constantTimeCompare(const std::string& a, const std::string& b) {
    if (a.length() != b.length()) {
        return false;
    }
    
    volatile uint8_t result = 0;
    for (size_t i = 0; i < a.length(); ++i) {
        result |= a[i] ^ b[i];
    }
    
    return result == 0;
}

bool constantTimeCompare(const std::vector<uint8_t>& a, const std::vector<uint8_t>& b) {
    if (a.size() != b.size()) {
        return false;
    }
    
    volatile uint8_t result = 0;
    for (size_t i = 0; i < a.size(); ++i) {
        result |= a[i] ^ b[i];
    }
    
    return result == 0;
}

int hammingDistance(const std::string& a, const std::string& b) {
    int distance = 0;
    size_t minLen = std::min(a.length(), b.length());
    
    for (size_t i = 0; i < minLen; ++i) {
        if (a[i] != b[i]) {
            distance++;
        }
    }
    
    distance += std::abs(static_cast<int>(a.length()) - static_cast<int>(b.length()));
    
    return distance;
}

double bitDifferencePercentage(const std::string& hash1, const std::string& hash2) {
    if (hash1.empty() || hash2.empty()) {
        return 0.0;
    }
    
    size_t minLen = std::min(hash1.length(), hash2.length());
    int differentBits = 0;
    int totalBits = minLen * 4; // Each hex char = 4 bits
    
    for (size_t i = 0; i < minLen; ++i) {
        uint8_t val1 = (hash1[i] >= '0' && hash1[i] <= '9') ? hash1[i] - '0' :
                       (hash1[i] >= 'a' && hash1[i] <= 'f') ? hash1[i] - 'a' + 10 :
                       (hash1[i] >= 'A' && hash1[i] <= 'F') ? hash1[i] - 'A' + 10 : 0;
        
        uint8_t val2 = (hash2[i] >= '0' && hash2[i] <= '9') ? hash2[i] - '0' :
                       (hash2[i] >= 'a' && hash2[i] <= 'f') ? hash2[i] - 'a' + 10 :
                       (hash2[i] >= 'A' && hash2[i] <= 'F') ? hash2[i] - 'A' + 10 : 0;
        
        uint8_t xorResult = val1 ^ val2;
        
        // Count set bits
        while (xorResult) {
            differentBits += xorResult & 1;
            xorResult >>= 1;
        }
    }
    
    return static_cast<double>(differentBits) / totalBits;
}

// ============================================================================
// STATISTICS AND ANALYSIS
// ============================================================================

double calculateMean(const std::vector<double>& values) {
    if (values.empty()) return 0.0;
    return std::accumulate(values.begin(), values.end(), 0.0) / values.size();
}

double calculateStdDev(const std::vector<double>& values) {
    if (values.empty()) return 0.0;
    
    double mean = calculateMean(values);
    double variance = 0.0;
    
    for (double val : values) {
        variance += (val - mean) * (val - mean);
    }
    
    variance /= values.size();
    return std::sqrt(variance);
}

double calculateMedian(std::vector<double> values) {
    if (values.empty()) return 0.0;
    
    std::sort(values.begin(), values.end());
    size_t n = values.size();
    
    if (n % 2 == 0) {
        return (values[n/2 - 1] + values[n/2]) / 2.0;
    } else {
        return values[n/2];
    }
}

std::string formatTime(double milliseconds) {
    std::stringstream ss;
    
    if (milliseconds < 1.0) {
        ss << std::fixed << std::setprecision(2) << (milliseconds * 1000.0) << " μs";
    } else if (milliseconds < 1000.0) {
        ss << std::fixed << std::setprecision(2) << milliseconds << " ms";
    } else {
        ss << std::fixed << std::setprecision(2) << (milliseconds / 1000.0) << " s";
    }
    
    return ss.str();
}

std::string formatBytes(size_t bytes) {
    std::stringstream ss;
    
    if (bytes < 1024) {
        ss << bytes << " B";
    } else if (bytes < 1024 * 1024) {
        ss << std::fixed << std::setprecision(2) << (bytes / 1024.0) << " KB";
    } else if (bytes < 1024 * 1024 * 1024) {
        ss << std::fixed << std::setprecision(2) << (bytes / (1024.0 * 1024.0)) << " MB";
    } else {
        ss << std::fixed << std::setprecision(2) << (bytes / (1024.0 * 1024.0 * 1024.0)) << " GB";
    }
    
    return ss.str();
}

// ============================================================================
// HELPER FUNCTIONS
// ============================================================================

void printSeparator(char c, int length) {
    std::cout << std::string(length, c) << std::endl;
}

void printHeader(const std::string& title) {
    printSeparator('=', 80);
    std::cout << "  " << title << std::endl;
    printSeparator('=', 80);
}

std::string getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&now_c), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

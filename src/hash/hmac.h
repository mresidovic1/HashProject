#pragma once
#include <string>

// HMAC (Hash-based Message Authentication Code)
// RFC 2104 implementacija za autentifikaciju i verifikaciju integriteta
class HMAC {
public:
    // Generisanje HMAC koda koristeći SHA-256
    static std::string generate(const std::string& key, const std::string& message);
    
    // Verifikacija HMAC koda
    static bool verify(const std::string& key, const std::string& message, 
                      const std::string& expectedHMAC);
    
    // Generisanje HMAC-a za fajl (verifikacija integriteta)
    static std::string generateForFile(const std::string& key, const std::string& filePath);
    
    // Constant-time poređenje (zaštita od timing napada)
    static bool constantTimeCompare(const std::string& a, const std::string& b);
};

#pragma once
#include <cstdint>
#include <string>
#include <vector>

// PBKDF2 (Password-Based Key Derivation Function 2)
// Implementacija RFC 2898 za sigurnu derivaciju ključeva iz lozinki
class PBKDF2 {
public:
    // Generisanje hash-a iz lozinke sa salt-om i brojem iteracija
    static std::string derive(
        const std::string& password,
        const std::string& salt,
        int iterations = 100000,
        int keyLength = 32
    );
    
    // Verifikacija lozinke protiv prethodno generisanog hash-a
    static bool verify(
        const std::string& password,
        const std::string& salt,
        const std::string& expectedHash,
        int iterations = 100000
    );
    
private:
    static std::vector<uint8_t> hmacSHA256(
        const std::vector<uint8_t>& key,
        const std::vector<uint8_t>& data
    );
};

#pragma once
#include <string>
#include <cstdint>

std::string generateSalt(int length = 16);
std::string generatePepper(int length = 16);
std::string generateRandomPassword(int length = 12);
uint64_t currentTimeMillis();

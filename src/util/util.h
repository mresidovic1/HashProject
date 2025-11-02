#pragma once
#include <string>
#include <random>
#include <chrono>

std::string generateSalt(int length=16);
std::string generatePepper(int length=16);
uint64_t currentTimeMillis();
std::string generateRandomPassword(int length=12); 
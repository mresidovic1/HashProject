#pragma once
#include <string>

std::string iterativeHash(const std::string &input, int iterations = 1000);
std::string simpleHash(const std::string &input);
std::string optimizedSHA256(const std::string &data);

#pragma once
#include "sha256.h"
#include <string>

std::string iterativeHash(const std::string &input, int iterations=10000);
std::string simpleHash(const std::string &input); 
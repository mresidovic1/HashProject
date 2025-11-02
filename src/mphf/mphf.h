#pragma once
#include <unordered_map>
#include <string>
#include <vector>

class MPHF {
public:
    void build(const std::vector<std::string>& keys);
    int lookup(const std::string& key) const;
private:
    std::unordered_map<std::string,int> map;
};
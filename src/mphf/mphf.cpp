#include "mphf.h"

void MPHF::build(const std::vector<std::string>& keys){
    for(size_t i=0;i<keys.size();i++)
        map[keys[i]] = i;
}

int MPHF::lookup(const std::string& key) const{
    auto it = map.find(key);
    if(it != map.end()) return it->second;
    return -1;
}

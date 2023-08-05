#pragma once

#include <unordered_map>

typedef const char* HxString;

template<typename KeyType, typename ValueType>
bool contains(const std::unordered_map<KeyType, ValueType>& map, const KeyType& key) {
    return map.find(key) != map.end();
}
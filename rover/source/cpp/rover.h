#pragma once

#include <unordered_set>
#include <unordered_map>

typedef const char* HxString;

template<typename T> bool contains(const std::unordered_set<T>& set, const T& key) {
    return set.find(key) != set.end();
}
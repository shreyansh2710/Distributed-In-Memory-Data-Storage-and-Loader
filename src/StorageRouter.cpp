// src/StorageRouter.cpp
#include "StorageRouter.hpp"
#include <functional>

StorageRouter::StorageRouter(int total_nodes) : node_count(total_nodes) {}

int StorageRouter::get_owner_node_id(const std::string& key) const {
    size_t hash_val = std::hash<std::string>{}(key);
    return static_cast<int>(hash_val % node_count);
}

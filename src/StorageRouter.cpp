#include "StorageRouter.hpp"
#include <algorithm>
#include <functional>

StorageRouter::StorageRouter(const std::vector<int>& node_ids) : cluster_nodes(node_ids) {
    std::sort(cluster_nodes.begin(), cluster_nodes.end());
}

int StorageRouter::get_owner_node_id(const std::string& key) const {
    if (cluster_nodes.empty()) return -1;
    size_t hash_val = std::hash<std::string>{}(key);
    return cluster_nodes[hash_val % cluster_nodes.size()];
}

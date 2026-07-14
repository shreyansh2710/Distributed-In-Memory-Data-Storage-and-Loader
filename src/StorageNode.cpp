#include "StorageNode.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <mutex>

StorageNode::StorageNode(int id, std::shared_ptr<StorageRouter> router) 
    : node_id(id), cluster_router(router) {}

int StorageNode::get_id() const { return node_id; }

size_t StorageNode::get_storage_count() const { 
    std::shared_lock<std::shared_mutex> lock(storage_mutex);
    return kv_store.size(); 
}

void StorageNode::direct_insert(const Record& record) {
    std::unique_lock<std::shared_mutex> lock(storage_mutex);
    kv_store[record.key] = record.value;
}

void StorageNode::read_and_partition_local_file(const std::string& filepath, 
                                               std::unordered_map<int, std::vector<Record>>& outbound_buffers) {
    std::ifstream file(filepath);
    if (!file.is_open()) return;

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        
        std::stringstream ss(line);
        std::string key, value;
        if (std::getline(ss, key, ',') && std::getline(ss, value)) {
            int target_node = cluster_router->get_owner_node_id(key);
            if (target_node == node_id) {
                direct_insert({key, value});
            } else {
                outbound_buffers[target_node].push_back({key, value});
            }
        }
    }
}

void StorageNode::accept_network_batch(const std::vector<Record>& batch) {
    std::unique_lock<std::shared_mutex> lock(storage_mutex);
    for (const auto& record : batch) {
        kv_store[record.key] = record.value;
    }
}

void StorageNode::print_debug_inventory() const {
    std::shared_lock<std::shared_mutex> lock(storage_mutex);
    std::cout << "Node [" << node_id << "] Inventory: ";
    for (auto const& [key, val] : kv_store) {
        std::cout << "(" << key << " -> Node " << cluster_router->get_owner_node_id(key) << ") ";
    }
    std::cout << "\n";
}

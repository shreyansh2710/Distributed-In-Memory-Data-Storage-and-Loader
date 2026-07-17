// src/StorageNode.cpp
#include "StorageNode.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

StorageNode::StorageNode(int id, std::shared_ptr<StorageRouter> router, const std::vector<FieldConfig>& schema) 
    : node_id(id), cluster_router(router), field_schema(schema) {}

size_t StorageNode::get_storage_count() const { 
    std::shared_lock<std::shared_mutex> lock(node_mutex);
    return kv_store.size(); 
}

void StorageNode::direct_insert(Record rec) {
    std::unique_lock<std::shared_mutex> lock(node_mutex);
    if (seen_keys.find(rec.key) == seen_keys.end()) {
        seen_keys.insert(rec.key);
        kv_store[rec.key] = std::move(rec.values);
    }
}

void StorageNode::process_local_file_stream(const std::string& filepath, std::unordered_map<int, std::vector<Record>>& outbound_buffers) {
    std::ifstream file(filepath, std::ios::binary | std::ios::ate);
    if (!file.is_open()) return;

    // Fast memory sizing and scanning optimization
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    std::vector<char> buffer(size);
    if (!file.read(buffer.data(), size)) return;

    std::string_view file_content(buffer.data(), size);
    size_t pos = 0;
    
    while (pos < file_content.size()) {
        size_t next_newline = file_content.find('\n', pos);
        if (next_newline == std::string_view::npos) next_newline = file_content.size();
        
        std::string_view line = file_content.substr(pos, next_newline - pos);
        pos = next_newline + 1;

        if (line.empty() || line[0] == '#') continue;

        // Fast low-overhead token splitting
        size_t comma_pos = line.find(',');
        if (comma_pos == std::string_view::npos) continue;

        std::string key(line.substr(0, comma_pos));
        std::string_view values_slice = line.substr(comma_pos + 1);

        // Deduplication optimization: drop instantly if seen locally
        {
            std::shared_lock<std::shared_mutex> lock(node_mutex);
            if (seen_keys.find(key) != seen_keys.end()) continue;
        }

        Record rec;
        rec.key = std::move(key);
        std::stringstream ss_vals((std::string(values_slice)));
        std::string val_token;

        for (const auto& config : field_schema) {
            if (!std::getline(ss_vals, val_token, ',')) break;
            if (config.type == DataType::INT32) {
                rec.values.emplace_back(std::stoi(val_token));
            } else {
                rec.values.emplace_back(val_token);
            }
        }

        int owner = cluster_router->get_owner_node_id(rec.key);
        if (owner == node_id) {
            direct_insert(std::move(rec));
        } else {
            outbound_buffers[owner].push_back(std::move(rec));
        }
    }
}

void StorageNode::print_inventory_sample() const {
    std::shared_lock<std::shared_mutex> lock(node_mutex);
    int count = 0;
    std::cout << "Node [" << node_id << "] Sample Inventory Logs:\n";
    for (const auto& [k, v] : kv_store) {
        std::cout << "  Key: " << k << " -> Tracked on Node: " << cluster_router->get_owner_node_id(k) << "\n";
        if (++count >= 3) break; // Sample print out to prevent flooding
    }
}

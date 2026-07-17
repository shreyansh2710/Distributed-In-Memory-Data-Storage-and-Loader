#ifndef STORAGE_NODE_HPP
#define STORAGE_NODE_HPP

#include "Record.hpp" 
#include "Types.hpp"
#include "StorageRouter.hpp"
#include "ConfigParser.hpp"
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <memory>
#include <shared_mutex>

class StorageNode {
public:
    StorageNode(int id, std::shared_ptr<StorageRouter> router, const std::vector<FieldConfig>& schema);
    
    int get_id() const { return node_id; }
    size_t get_storage_count() const;
    
    void direct_insert(Record rec);
    void process_local_file_stream(const std::string& filepath, std::unordered_map<int, std::vector<Record>>& outbound_buffers);
    void print_inventory_sample() const;

private:
    int node_id;
    std::shared_ptr<StorageRouter> cluster_router;
    std::vector<FieldConfig> field_schema;
    
    // Core internal map storing uniquely-owned values
    std::unordered_map<std::string, std::vector<FieldValue>> kv_store;
    
    // In-memory deduplication index tracking local uniqueness records
    std::unordered_set<std::string> seen_keys;
    
    mutable std::shared_mutex node_mutex;
};

#endif

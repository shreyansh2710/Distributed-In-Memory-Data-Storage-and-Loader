#ifndef STORAGE_NODE_HPP
#define STORAGE_NODE_HPP

#include "Record.hpp"
#include "StorageRouter.hpp"
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <shared_mutex>

class StorageNode {
public:
    StorageNode(int id, std::shared_ptr<StorageRouter> router);

    int get_id() const;
    size_t get_storage_count() const;
    
    void direct_insert(const Record& record);
    void read_and_partition_local_file(const std::string& filepath, 
                                       std::unordered_map<int, std::vector<Record>>& outbound_buffers);
    void accept_network_batch(const std::vector<Record>& batch);
    void print_debug_inventory() const;

private:
    int node_id;
    std::shared_ptr<StorageRouter> cluster_router;
    std::unordered_map<std::string, std::string> kv_store;
    mutable std::shared_mutex storage_mutex;
};

#endif

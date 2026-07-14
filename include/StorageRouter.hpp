#ifndef STORAGE_ROUTER_HPP
#define STORAGE_ROUTER_HPP

#include <vector>
#include <string>

class StorageRouter {
public:
    explicit StorageRouter(const std::vector<int>& node_ids);
    int get_owner_node_id(const std::string& key) const;

private:
    std::vector<int> cluster_nodes;
};

#endif

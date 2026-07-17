#ifndef STORAGE_ROUTER_HPP
#define STORAGE_ROUTER_HPP

#include <string>
#include <vector>

class StorageRouter {
public:
    explicit StorageRouter(int total_nodes);
    int get_owner_node_id(const std::string& key) const;
private:
    int node_count;
};

#endif

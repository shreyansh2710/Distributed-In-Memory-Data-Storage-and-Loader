#ifndef CLUSTER_COORDINATOR_HPP
#define CLUSTER_COORDINATOR_HPP

#include "StorageNode.hpp"
#include "StorageRouter.hpp"
#include <vector>
#include <unordered_map>
#include <memory>

class ClusterCoordinator {
public:
    explicit ClusterCoordinator(const std::vector<int>& node_ids);
    void run_distributed_load(const std::unordered_map<int, std::string>& node_file_maps);
    void print_cluster_metrics() const;

private:
    std::shared_ptr<StorageRouter> router;
    std::unordered_map<int, std::unique_ptr<StorageNode>> nodes;
};

#endif

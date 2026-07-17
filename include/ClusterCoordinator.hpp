#ifndef CLUSTER_COORDINATOR_HPP
#define CLUSTER_COORDINATOR_HPP

#include "StorageNode.hpp"
#include "StorageRouter.hpp"
#include "ConfigParser.hpp"
#include <vector>
#include <memory>
#include <string>

class ClusterCoordinator {
public:
    explicit ClusterCoordinator(const ConfigParser& config);
    void execute_parallel_load(const std::vector<std::string>& files);
    void show_cluster_diagnostics() const;

private:
    int total_nodes;
    std::shared_ptr<StorageRouter> router;
    std::vector<std::unique_ptr<StorageNode>> nodes;
};

#endif

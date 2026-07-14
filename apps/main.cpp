#include "ClusterCoordinator.hpp"
#include <fstream>
#include <iostream>

void create_mock_file(const std::string& path, const std::vector<std::pair<std::string, std::string>>& items) {
    std::ofstream f(path);
    for (const auto& [k, v] : items) {
        f << k << "," << v << "\n";
    }
}

int main() {
    create_mock_file("node0_data.csv", { {"usr_10", "Alice"}, {"usr_22", "Bob"}, {"usr_03", "Charlie"} });
    create_mock_file("node1_data.csv", { {"usr_94", "David"}, {"usr_45", "Eve"}, {"usr_12", "Frank"} });
    create_mock_file("node2_data.csv", { {"usr_87", "Grace"}, {"usr_33", "Heidi"}, {"usr_01", "Ivan"} });

    std::vector<int> node_ids = {0, 1, 2};
    ClusterCoordinator cluster(node_ids);

    std::unordered_map<int, std::string> node_file_maps = {
        {0, "node0_data.csv"},
        {1, "node1_data.csv"},
        {2, "node2_data.csv"}
    };

    cluster.run_distributed_load(node_file_maps);
    cluster.print_cluster_metrics();

    std::remove("node0_data.csv");
    std::remove("node1_data.csv");
    std::remove("node2_data.csv");

    return 0;
}

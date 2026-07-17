#include "ConfigParser.hpp"
#include "ClusterCoordinator.hpp"
#include <iostream>
#include <chrono>

int main() {
    auto start_time = std::chrono::high_resolution_clock::now();

    ConfigParser config("config.txt");
    ClusterCoordinator cluster(config);

    // Collect list of generated large mock data files
    std::vector<std::string> mock_files;
    for(int i = 0; i < config.get_node_count(); ++i) {
        mock_files.push_back("mock_node_" + std::to_string(i) + "_data.csv");
    }

    cluster.execute_parallel_load(mock_files);
    cluster.show_cluster_diagnostics();

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    std::cout << "\n[Perf Trace] Complete Cluster Execution Duration: " << duration << " ms\n";

    return 0;
}

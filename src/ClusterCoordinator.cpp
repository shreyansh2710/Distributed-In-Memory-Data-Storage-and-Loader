#include "ClusterCoordinator.hpp"
#include <iostream>
#include <iomanip>

ClusterCoordinator::ClusterCoordinator(const std::vector<int>& node_ids) {
    router = std::make_shared<StorageRouter>(node_ids);
    for (int id : node_ids) {
        nodes[id] = std::make_unique<StorageNode>(id, router);
    }
}

void ClusterCoordinator::run_distributed_load(const std::unordered_map<int, std::string>& node_file_maps) {
    std::cout << "[Cluster] Initiating Distributed Load Job...\n\n";
    std::vector<NetworkMessage> network_bus;

    for (auto& [node_id, node_ptr] : nodes) {
        auto file_it = node_file_maps.find(node_id);
        if (file_it != node_file_maps.end()) {
            std::unordered_map<int, std::vector<Record>> outbound_buffers;
            node_ptr->read_and_partition_local_file(file_it->second, outbound_buffers);
            
            for (auto& [target_node, records] : outbound_buffers) {
                if (!records.empty()) {
                    network_bus.push_back({node_id, target_node, std::move(records)});
                }
            }
        }
    }

    std::cout << "[Network] Simulating transit of " << network_bus.size() << " payload batches...\n";
    for (const auto& msg : network_bus) {
        std::cout << "  -> Transferring " << msg.payload.size() << " records from Node " 
                  << msg.sender_node_id << " to Owner Node " << msg.recipient_node_id << "\n";
        nodes[msg.recipient_node_id]->accept_network_batch(msg.payload);
    }
    std::cout << "[Network] Communication fabrics cleared.\n\n";
}

void ClusterCoordinator::print_cluster_metrics() const {
    std::cout << "=========================================\n";
    std::cout << "        CLUSTER LOADING STATISTICS       \n";
    std::cout << "=========================================\n";
    std::cout << std::left << std::setw(12) << "Node ID" << "Records Stored\n";
    std::cout << "-----------------------------------------\n";
    
    size_t total_records = 0;
    for (const auto& [id, node_ptr] : nodes) {
        size_t count = node_ptr->get_storage_count();
        total_records += count;
        std::cout << std::left << std::setw(12) << id << count << "\n";
    }
    std::cout << "-----------------------------------------\n";
    std::cout << std::left << std::setw(12) << "Total" << total_records << "\n\n";

    for (const auto& [id, node_ptr] : nodes) {
        node_ptr->print_debug_inventory();
    }
}

// src/ClusterCoordinator.cpp
#include "ClusterCoordinator.hpp"
#include "NetworkMock.hpp"
#include <iostream>
#include <thread>
#include <future>

ClusterCoordinator::ClusterCoordinator(const ConfigParser& config) : total_nodes(config.get_node_count()) {
    router = std::make_shared<StorageRouter>(total_nodes);
    for (int i = 0; i < total_nodes; ++i) {
        nodes.push_back(std::make_unique<StorageNode>(i, router, config.get_schema()));
    }
}

void ClusterCoordinator::execute_parallel_load(const std::vector<std::string>& files) {
    std::cout << "[Cluster] Initializing Server Nodes Processes concurrently...\n";
    std::vector<std::unordered_map<int, std::vector<Record>>> parallel_outbound_buffers(total_nodes);
    std::vector<std::future<void>> worker_threads;

    // Phase 1: Local Reading & Concurrent Filtering/Partitioning
    for (int i = 0; i < total_nodes; ++i) {
        if (i >= static_cast<int>(files.size())) break;
        worker_threads.push_back(std::async(std::launch::async, [this, i, &files, &parallel_outbound_buffers]() {
            nodes[i]->process_local_file_stream(files[i], parallel_outbound_buffers[i]);
        }));
    }
    for (auto& fut : worker_threads) fut.get();

    // Phase 2: Packing buffers & Driving Serialized Mock Sockets network layer
    std::cout << "[Network] Packing records into serialized binary blocks over sockets...\n";
    for (int src = 0; src < total_nodes; ++src) {
        for (auto& [dest, records] : parallel_outbound_buffers[src]) {
            if (records.empty()) continue;
            NetworkMock::connect(src, dest);
            std::vector<uint8_t> payload = BinarySerializer::serialize(records);
            NetworkMock::send(dest, payload);
        }
    }

    // Phase 3: Recv and Direct Ingestion
    for (int dest = 0; dest < total_nodes; ++dest) {
        while (true) {
            std::vector<uint8_t> inbound_bytes = NetworkMock::recv(dest);
            if (inbound_bytes.empty()) break;
            std::vector<Record> inbound_records = BinarySerializer::deserialize(inbound_bytes);
            for (auto& rec : inbound_records) {
                nodes[dest]->direct_insert(std::move(rec));
            }
        }
    }
    std::cout << "[Network] Sockets cleared. Ingestion phase terminated.\n\n";
}

void ClusterCoordinator::show_cluster_diagnostics() const {
    std::cout << "=========================================\n";
    std::cout << "        CLUSTER DISK INGESTION STATS     \n";
    std::cout << "=========================================\n";
    size_t total = 0;
    for (int i = 0; i < total_nodes; ++i) {
        size_t node_count = nodes[i]->get_storage_count();
        total += node_count;
        std::cout << "Node ID [" << i << "] uniquely owns: " << node_count << " records\n";
    }
    std::cout << "-----------------------------------------\n";
    std::cout << "Global Total Storage Footprint: " << total << " items\n\n";

    std::cout << "=========================================\n";
    std::cout << "      DATA PROOF OWNERSHIP VALIDATION    \n";
    std::cout << "=========================================\n";
    for (int i = 0; i < total_nodes; ++i) {
        nodes[i]->print_inventory_sample();
    }
}

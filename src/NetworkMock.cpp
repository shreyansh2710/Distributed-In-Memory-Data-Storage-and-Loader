// src/NetworkMock.cpp
#include "NetworkMock.hpp"

std::unordered_map<int, std::queue<std::vector<uint8_t>>> NetworkMock::network_queues;
std::mutex NetworkMock::net_mutex;

void NetworkMock::connect(int source_node, int target_node) {
    // Validates route topology channel openings without execution delay
}

void NetworkMock::send(int target_node, const std::vector<uint8_t>& data) {
    std::lock_guard<std::mutex> lock(net_mutex);
    network_queues[target_node].push(data);
}

std::vector<uint8_t> NetworkMock::recv(int target_node) {
    std::lock_guard<std::mutex> lock(net_mutex);
    auto& q = network_queues[target_node];
    if (q.empty()) return {};
    std::vector<uint8_t> data = std::move(q.front());
    q.pop();
    return data;
}

void NetworkMock::clear_fabric() {
    std::lock_guard<std::mutex> lock(net_mutex);
    network_queues.clear();
}

#ifndef NETWORK_MOCK_HPP
#define NETWORK_MOCK_HPP

#include <vector>
#include <cstdint>
#include <unordered_map>
#include <queue>
#include <mutex>

// Simulates POSIX/Berkeley primitive socket interactions using byte sequences
class NetworkMock {
public:
    static void connect(int source_node, int target_node);
    static void send(int target_node, const std::vector<uint8_t>& data);
    static std::vector<uint8_t> recv(int target_node);
    static void clear_fabric();

private:
    static std::unordered_map<int, std::queue<std::vector<uint8_t>>> network_queues;
    static std::mutex net_mutex;
};

#endif

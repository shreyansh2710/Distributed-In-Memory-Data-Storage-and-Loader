#ifndef RECORD_HPP
#define RECORD_HPP

#include <string>
#include <vector>

struct Record {
    std::string key;
    std::string value;
};

struct NetworkMessage {
    int sender_node_id;
    int recipient_node_id;
    std::vector<Record> payload;
};

#endif

// src/ConfigParser.cpp
#include "ConfigParser.hpp"
#include <fstream>
#include <sstream>

ConfigParser::ConfigParser(const std::string& config_path) {
    std::ifstream file(config_path);
    if (!file.is_open()) return;
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::stringstream ss(line);
        std::string key, val;
        if (std::getline(ss, key, '=') && std::getline(ss, val)) {
            if (key == "nodes") {
                node_count = std::stoi(val);
            } else if (key == "field") {
                std::stringstream field_ss(val);
                std::string f_name, f_type;
                if (std::getline(field_ss, f_name, ',') && std::getline(field_ss, f_type)) {
                    DataType t = (f_type == "int32") ? DataType::INT32 : DataType::STRING;
                    schema.push_back({f_name, t});
                }
            }
        }
    }
}

#ifndef CONFIG_PARSER_HPP
#define CONFIG_PARSER_HPP

#include <string>
#include <vector>

enum class DataType { STRING, INT32 };

struct FieldConfig {
    std::string name;
    DataType type;
};

class ConfigParser {
public:
    explicit ConfigParser(const std::string& config_path);
    int get_node_count() const { return node_count; }
    const std::vector<FieldConfig>& get_schema() const { return schema; }

private:
    int node_count = 1;
    std::vector<FieldConfig> schema;
};

#endif

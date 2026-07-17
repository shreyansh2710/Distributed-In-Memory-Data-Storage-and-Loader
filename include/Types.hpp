#ifndef TYPES_HPP
#define TYPES_HPP

#include <string>
#include <vector>
#include <variant>
#include <cstdint>
#include <stdexcept>
#include <cstring>

using FieldValue = std::variant<std::string, int32_t>;

struct Record {
    std::string key;
    std::vector<FieldValue> values;
};

// High-speed binary serializer maximizing throughput over the wire
class BinarySerializer {
public:
    static std::vector<uint8_t> serialize(const std::vector<Record>& records) {
        std::vector<uint8_t> buffer;
        uint64_t num_records = records.size();
        append_to_buffer(buffer, &num_records, sizeof(num_records));

        for (const auto& rec : records) {
            uint32_t key_len = static_cast<uint32_t>(rec.key.size());
            append_to_buffer(buffer, &key_len, sizeof(key_len));
            append_to_buffer(buffer, rec.key.data(), key_len);

            uint32_t num_vals = static_cast<uint32_t>(rec.values.size());
            append_to_buffer(buffer, &num_vals, sizeof(num_vals));

            for (const auto& val : rec.values) {
                uint8_t type_idx = static_cast<uint8_t>(val.index());
                append_to_buffer(buffer, &type_idx, sizeof(type_idx));
                
                if (type_idx == 0) { // string
                    const auto& str = std::get<std::string>(val);
                    uint32_t str_len = static_cast<uint32_t>(str.size());
                    append_to_buffer(buffer, &str_len, sizeof(str_len));
                    append_to_buffer(buffer, str.data(), str_len);
                } else { // int32_t
                    int32_t num = std::get<int32_t>(val);
                    append_to_buffer(buffer, &num, sizeof(num));
                }
            }
        }
        return buffer;
    }

    static std::vector<Record> deserialize(const std::vector<uint8_t>& buffer) {
        std::vector<Record> records;
        if (buffer.empty()) return records;
        
        size_t offset = 0;
        uint64_t num_records = 0;
        read_from_buffer(buffer, offset, &num_records, sizeof(num_records));
        records.reserve(num_records);

        for (uint64_t i = 0; i < num_records; ++i) {
            Record rec;
            uint32_t key_len = 0;
            read_from_buffer(buffer, offset, &key_len, sizeof(key_len));
            rec.key.resize(key_len);
            read_from_buffer(buffer, offset, &rec.key[0], key_len);

            uint32_t num_vals = 0;
            read_from_buffer(buffer, offset, &num_vals, sizeof(num_vals));
            rec.values.reserve(num_vals);

            for (uint32_t j = 0; j < num_vals; ++j) {
                uint8_t type_idx = 0;
                read_from_buffer(buffer, offset, &type_idx, sizeof(type_idx));
                
                if (type_idx == 0) {
                    uint32_t str_len = 0;
                    read_from_buffer(buffer, offset, &str_len, sizeof(str_len));
                    std::string str(str_len, '\0');
                    read_from_buffer(buffer, offset, &str[0], str_len);
                    rec.values.emplace_back(str);
                } else {
                    int32_t num = 0;
                    read_from_buffer(buffer, offset, &num, sizeof(num));
                    rec.values.emplace_back(num);
                }
            }
            records.push_back(std::move(rec));
        }
        return records;
    }

private:
    static void append_to_buffer(std::vector<uint8_t>& buf, const void* data, size_t size) {
        const uint8_t* ptr = reinterpret_cast<const uint8_t*>(data);
        buf.insert(buf.end(), ptr, ptr + size);
    }

    static void read_from_buffer(const std::vector<uint8_t>& buf, size_t& offset, void* dest, size_t size) {
        if (offset + size > buf.size()) throw std::runtime_error("Malformed buffer read out of bounds.");
        std::memcpy(dest, buf.data() + offset, size);
        offset += size;
    }
};

#endif

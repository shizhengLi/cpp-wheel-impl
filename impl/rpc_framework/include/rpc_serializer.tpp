#pragma once

#include "rpc_framework.hpp"
#include <sstream>
#include <iomanip>
#include <type_traits>

namespace rpc {

/**
 * @brief 二进制序列化器
 */
class BinarySerializer : public Serializer {
private:
    std::ostringstream buffer_;
    std::istringstream input_buffer_;
    
public:
    BinarySerializer() : buffer_(std::ios::binary) {}
    
    void serialize(const std::string& data) override {
        buffer_.write(data.c_str(), data.size());
    }
    
    std::string deserialize() override {
        std::string result;
        std::getline(input_buffer_, result, '\0');
        return result;
    }
    
    void reset() override {
        buffer_.str("");
        buffer_.clear();
        input_buffer_.str("");
        input_buffer_.clear();
    }
    
    std::string get_data() const {
        return buffer_.str();
    }
    
    void set_data(const std::string& data) {
        input_buffer_.str(data);
        input_buffer_.clear();
    }
};

/**
 * @brief JSON序列化器
 */
class JsonSerializer : public Serializer {
private:
    std::ostringstream buffer_;
    std::string input_data_;
    size_t input_pos_;
    
public:
    JsonSerializer() : input_pos_(0) {}
    
    void serialize(const std::string& data) override {
        buffer_ << "\"" << escape_json(data) << "\"";
    }
    
    std::string deserialize() override {
        if (input_data_.empty() || input_pos_ >= input_data_.size()) {
            return "";
        }
        
        // 跳过空白字符
        while (input_pos_ < input_data_.size() && 
               (input_data_[input_pos_] == ' ' || input_data_[input_pos_] == '\t' || 
                input_data_[input_pos_] == '\n' || input_data_[input_pos_] == '\r')) {
            input_pos_++;
        }
        
        if (input_pos_ >= input_data_.size()) {
            return "";
        }
        
        // 处理字符串
        if (input_data_[input_pos_] == '"') {
            input_pos_++;
            std::string result;
            while (input_pos_ < input_data_.size() && input_data_[input_pos_] != '"') {
                if (input_data_[input_pos_] == '\\') {
                    input_pos_++;
                    if (input_pos_ < input_data_.size()) {
                        switch (input_data_[input_pos_]) {
                            case '"': result += '"'; break;
                            case '\\': result += '\\'; break;
                            case '/': result += '/'; break;
                            case 'b': result += '\b'; break;
                            case 'f': result += '\f'; break;
                            case 'n': result += '\n'; break;
                            case 'r': result += '\r'; break;
                            case 't': result += '\t'; break;
                            default: result += input_data_[input_pos_]; break;
                        }
                        input_pos_++;
                    }
                } else {
                    result += input_data_[input_pos_];
                    input_pos_++;
                }
            }
            if (input_pos_ < input_data_.size() && input_data_[input_pos_] == '"') {
                input_pos_++;
            }
            return result;
        }
        
        // 处理数字
        std::string num_str;
        while (input_pos_ < input_data_.size() && 
               (input_data_[input_pos_] == '-' || input_data_[input_pos_] == '.' || 
                (input_data_[input_pos_] >= '0' && input_data_[input_pos_] <= '9'))) {
            num_str += input_data_[input_pos_];
            input_pos_++;
        }
        return num_str;
    }
    
    void reset() override {
        buffer_.str("");
        buffer_.clear();
        input_data_.clear();
        input_pos_ = 0;
    }
    
    std::string get_data() const {
        return buffer_.str();
    }
    
    void set_data(const std::string& data) {
        input_data_ = data;
        input_pos_ = 0;
    }
    
private:
    std::string escape_json(const std::string& input) {
        std::string result;
        for (char c : input) {
            switch (c) {
                case '"': result += "\\\""; break;
                case '\\': result += "\\\\"; break;
                case '\b': result += "\\b"; break;
                case '\f': result += "\\f"; break;
                case '\n': result += "\\n"; break;
                case '\r': result += "\\r"; break;
                case '\t': result += "\\t"; break;
                default:
                    if (c >= 0 && c < 32) {
                        char buffer[7];
                        snprintf(buffer, sizeof(buffer), "\\u%04x", c);
                        result += buffer;
                    } else {
                        result += c;
                    }
                    break;
            }
        }
        return result;
    }
};

/**
 * @brief 序列化工具函数
 */
class SerializationUtils {
public:
    // 序列化基本类型
    template<typename T>
    static std::string serialize_basic(const T& value) {
        static_assert(std::is_arithmetic_v<T>, "Type must be arithmetic");
        
        if constexpr (std::is_same_v<T, bool>) {
            return value ? "true" : "false";
        } else if constexpr (std::is_integral_v<T>) {
            return std::to_string(value);
        } else if constexpr (std::is_floating_point_v<T>) {
            return std::to_string(value);
        }
    }
    
    // 反序列化基本类型
    template<typename T>
    static T deserialize_basic(const std::string& str) {
        static_assert(std::is_arithmetic_v<T>, "Type must be arithmetic");
        
        if constexpr (std::is_same_v<T, bool>) {
            return str == "true";
        } else if constexpr (std::is_integral_v<T>) {
            return static_cast<T>(std::stoll(str));
        } else if constexpr (std::is_floating_point_v<T>) {
            return static_cast<T>(std::stod(str));
        }
    }
    
    // 序列化字符串
    static std::string serialize_string(const std::string& str) {
        return str;
    }
    
    // 反序列化字符串
    static std::string deserialize_string(const std::string& str) {
        return str;
    }
    
    // 序列化向量
    template<typename T>
    static std::string serialize_vector(const std::vector<T>& vec) {
        std::ostringstream oss;
        oss << "[";
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) oss << ",";
            oss << serialize_basic(vec[i]);
        }
        oss << "]";
        return oss.str();
    }
    
    // 反序列化向量
    template<typename T>
    static std::vector<T> deserialize_vector(const std::string& str) {
        std::vector<T> result;
        if (str.empty() || str[0] != '[') {
            return result;
        }
        
        std::string content = str.substr(1, str.size() - 2);
        std::istringstream iss(content);
        std::string item;
        
        while (std::getline(iss, item, ',')) {
            result.push_back(deserialize_basic<T>(item));
        }
        
        return result;
    }
    
    // 序列化映射
    template<typename K, typename V>
    static std::string serialize_map(const std::map<K, V>& map) {
        std::ostringstream oss;
        oss << "{";
        bool first = true;
        for (const auto& [key, value] : map) {
            if (!first) oss << ",";
            if constexpr (std::is_same_v<K, std::string>) {
                oss << "\"" << serialize_string(key) << "\":" << serialize_basic(value);
            } else {
                oss << "\"" << serialize_basic(key) << "\":" << serialize_basic(value);
            }
            first = false;
        }
        oss << "}";
        return oss.str();
    }
    
    // 反序列化映射
    template<typename K, typename V>
    static std::map<K, V> deserialize_map(const std::string& str) {
        std::map<K, V> result;
        if (str.empty() || str[0] != '{') {
            return result;
        }
        
        std::string content = str.substr(1, str.size() - 2);
        std::istringstream iss(content);
        std::string pair;
        
        while (std::getline(iss, pair, ',')) {
            size_t colon_pos = pair.find(':');
            if (colon_pos != std::string::npos) {
                std::string key_str = pair.substr(0, colon_pos);
                std::string value_str = pair.substr(colon_pos + 1);
                
                // 移除引号
                if (key_str.size() >= 2 && key_str[0] == '"' && key_str[key_str.size()-1] == '"') {
                    key_str = key_str.substr(1, key_str.size() - 2);
                }
                
                K key;
                if constexpr (std::is_same_v<K, std::string>) {
                    key = deserialize_string(key_str);
                } else {
                    key = deserialize_basic<K>(key_str);
                }
                V value = deserialize_basic<V>(value_str);
                result[key] = value;
            }
        }
        
        return result;
    }
};

} // namespace rpc
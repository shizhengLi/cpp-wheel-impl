#pragma once

#include "rpc_framework.hpp"
#include <sstream>
#include <iomanip>

namespace rpc {

template<typename... Args>
std::string RpcClient::serialize_args(const Args&... args) {
    std::ostringstream oss;
    
    // 序列化参数数量
    size_t arg_count = sizeof...(Args);
    oss << std::hex << std::setw(8) << std::setfill('0') << arg_count;
    
    // 递归序列化每个参数
    auto serialize_one = [&oss](const auto& arg) {
        if constexpr (std::is_same_v<std::decay_t<decltype(arg)>, std::string>) {
            std::string arg_str = arg;
            oss << std::hex << std::setw(8) << std::setfill('0') << arg_str.size();
            oss << arg_str;
        } else if constexpr (std::is_same_v<std::decay_t<decltype(arg)>, const char*>) {
            std::string arg_str = arg;
            oss << std::hex << std::setw(8) << std::setfill('0') << arg_str.size();
            oss << arg_str;
        } else {
            std::string arg_str = std::to_string(arg);
            oss << std::hex << std::setw(8) << std::setfill('0') << arg_str.size();
            oss << arg_str;
        }
    };
    
    // 使用折叠表达式序列化所有参数
    (serialize_one(args), ...);
    
    return oss.str();
}

template<typename Ret>
Ret RpcClient::deserialize_result(const std::string& data) {
    if (data.empty()) {
        throw rpc_exception("Empty response data");
    }
    
    std::istringstream iss(data);
    
    // 读取结果长度
    std::string len_str = data.substr(0, 8);
    size_t result_len = std::stoul(len_str, nullptr, 16);
    
    if (data.size() < 8 + result_len) {
        throw rpc_exception("Invalid response data length");
    }
    
    // 读取结果值
    std::string result_str = data.substr(8, result_len);
    
    if constexpr (std::is_same_v<Ret, std::string>) {
        return result_str;
    } else if constexpr (std::is_integral_v<Ret>) {
        return static_cast<Ret>(std::stoll(result_str));
    } else if constexpr (std::is_floating_point_v<Ret>) {
        return static_cast<Ret>(std::stod(result_str));
    } else {
        throw rpc_exception("Unsupported return type");
    }
}

template<typename Ret, typename... Args>
Ret RpcClient::call(uint32_t service_id, uint32_t method_id, const Args&... args) {
    if (!is_connected()) {
        throw rpc_exception("Not connected to server");
    }
    
    // 序列化参数
    std::string serialized_args = serialize_args(args...);
    
    // 创建消息
    Message message;
    message.header.magic_number = 0x52504346; // "RPCF"
    message.header.message_id = next_message_id_++;
    message.header.message_type = static_cast<uint32_t>(MessageType::REQUEST);
    message.header.service_id = service_id;
    message.header.method_id = method_id;
    message.header.payload_size = serialized_args.size();
    message.header.sequence_id = 0;
    message.payload = serialized_args;
    
    // 创建promise用于等待响应
    std::promise<std::string> response_promise;
    auto response_future = response_promise.get_future();
    
    {
        std::lock_guard<std::mutex> lock(pending_mutex_);
        pending_calls_[message.header.message_id] = std::move(response_promise);
    }
    
    // 发送请求
    try {
        send_message(message);
    } catch (const std::exception& e) {
        std::lock_guard<std::mutex> lock(pending_mutex_);
        pending_calls_.erase(message.header.message_id);
        throw rpc_exception("Failed to send request: " + std::string(e.what()));
    }
    
    // 等待响应
    auto status = response_future.wait_for(std::chrono::seconds(30));
    if (status == std::future_status::timeout) {
        std::lock_guard<std::mutex> lock(pending_mutex_);
        pending_calls_.erase(message.header.message_id);
        throw rpc_exception("RPC call timeout");
    }
    
    // 获取响应
    std::string response_data = response_future.get();
    
    // 检查是否是错误响应
    if (response_data.size() >= 4 && response_data.substr(0, 4) == "ERR:") {
        throw rpc_exception("RPC error: " + response_data.substr(4));
    }
    
    // 反序列化结果
    return deserialize_result<Ret>(response_data);
}

template<typename Ret, typename... Args>
std::future<Ret> RpcClient::async_call(uint32_t service_id, uint32_t method_id, const Args&... args) {
    return std::async(std::launch::async, [this, service_id, method_id, args...]() {
        return call<Ret, Args...>(service_id, method_id, args...);
    });
}

} // namespace rpc
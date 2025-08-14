#include "rpc_framework.hpp"
#include <cstring>
#include <iostream>
#include <chrono>
#include <random>
#include <fcntl.h>

namespace rpc {

// 序列化消息头
std::string serialize_header(const MessageHeader& header) {
    std::string result(28, '\0'); // 7 * 4 bytes
    
    // 转换为网络字节序
    uint32_t magic = htonl(header.magic_number);
    uint32_t msg_id = htonl(header.message_id);
    uint32_t msg_type = htonl(header.message_type);
    uint32_t svc_id = htonl(header.service_id);
    uint32_t method_id = htonl(header.method_id);
    uint32_t payload_size = htonl(header.payload_size);
    uint32_t seq_id = htonl(header.sequence_id);
    
    memcpy(&result[0], &magic, 4);
    memcpy(&result[4], &msg_id, 4);
    memcpy(&result[8], &msg_type, 4);
    memcpy(&result[12], &svc_id, 4);
    memcpy(&result[16], &method_id, 4);
    memcpy(&result[20], &payload_size, 4);
    memcpy(&result[24], &seq_id, 4);
    
    return result;
}

// 反序列化消息头
MessageHeader deserialize_header(const std::string& data) {
    if (data.size() < 28) {
        throw rpc_exception("Invalid header data size");
    }
    
    MessageHeader header;
    
    uint32_t magic, msg_id, msg_type, svc_id, method_id, payload_size, seq_id;
    
    memcpy(&magic, &data[0], 4);
    memcpy(&msg_id, &data[4], 4);
    memcpy(&msg_type, &data[8], 4);
    memcpy(&svc_id, &data[12], 4);
    memcpy(&method_id, &data[16], 4);
    memcpy(&payload_size, &data[20], 4);
    memcpy(&seq_id, &data[24], 4);
    
    header.magic_number = ntohl(magic);
    header.message_id = ntohl(msg_id);
    header.message_type = ntohl(msg_type);
    header.service_id = ntohl(svc_id);
    header.method_id = ntohl(method_id);
    header.payload_size = ntohl(payload_size);
    header.sequence_id = ntohl(seq_id);
    
    return header;
}

// 序列化完整消息
std::string serialize_message(const Message& message) {
    std::string header_data = serialize_header(message.header);
    return header_data + message.payload;
}

// 反序列化完整消息
Message deserialize_message(const std::string& data) {
    if (data.size() < 28) {
        throw rpc_exception("Invalid message data size");
    }
    
    Message message;
    message.header = deserialize_header(data.substr(0, 28));
    
    if (!validate_header(message.header)) {
        throw rpc_exception("Invalid message header");
    }
    
    if (data.size() < 28 + message.header.payload_size) {
        throw rpc_exception("Invalid payload size");
    }
    
    message.payload = data.substr(28, message.header.payload_size);
    return message;
}

// 创建请求消息
Message create_request_message(uint32_t service_id, uint32_t method_id, 
                             uint32_t message_id, const std::string& payload) {
    Message message;
    message.header.magic_number = 0x52504346; // "RPCF"
    message.header.message_id = message_id;
    message.header.message_type = static_cast<uint32_t>(MessageType::REQUEST);
    message.header.service_id = service_id;
    message.header.method_id = method_id;
    message.header.payload_size = payload.size();
    message.header.sequence_id = 0;
    message.payload = payload;
    
    return message;
}

// 创建响应消息
Message create_response_message(uint32_t service_id, uint32_t method_id,
                              uint32_t message_id, const std::string& payload) {
    Message message;
    message.header.magic_number = 0x52504346; // "RPCF"
    message.header.message_id = message_id;
    message.header.message_type = static_cast<uint32_t>(MessageType::RESPONSE);
    message.header.service_id = service_id;
    message.header.method_id = method_id;
    message.header.payload_size = payload.size();
    message.header.sequence_id = 0;
    message.payload = payload;
    
    return message;
}

// 创建错误消息
Message create_error_message(uint32_t service_id, uint32_t method_id,
                           uint32_t message_id, const std::string& error_msg) {
    Message message;
    message.header.magic_number = 0x52504346; // "RPCF"
    message.header.message_id = message_id;
    message.header.message_type = static_cast<uint32_t>(MessageType::ERROR);
    message.header.service_id = service_id;
    message.header.method_id = method_id;
    message.header.payload_size = error_msg.size();
    message.header.sequence_id = 0;
    message.payload = error_msg;
    
    return message;
}

// 创建心跳消息
Message create_heartbeat_message(uint32_t message_id) {
    Message message;
    message.header.magic_number = 0x52504346; // "RPCF"
    message.header.message_id = message_id;
    message.header.message_type = static_cast<uint32_t>(MessageType::HEARTBEAT);
    message.header.service_id = 0;
    message.header.method_id = 0;
    message.header.payload_size = 0;
    message.header.sequence_id = 0;
    message.payload = "";
    
    return message;
}

// 生成消息ID
uint32_t generate_message_id() {
    static std::atomic<uint32_t> next_id(1);
    return next_id++;
}

// 验证消息头
bool validate_header(const MessageHeader& header) {
    return header.magic_number == 0x52504346; // "RPCF"
}

// 获取消息类型字符串
std::string get_message_type_string(MessageType type) {
    switch (type) {
        case MessageType::REQUEST: return "REQUEST";
        case MessageType::RESPONSE: return "RESPONSE";
        case MessageType::ERROR: return "ERROR";
        case MessageType::HEARTBEAT: return "HEARTBEAT";
        default: return "UNKNOWN";
    }
}

// 网络工具函数
namespace network_utils {

// 设置socket超时
void set_socket_timeout(int socket_fd, int timeout_seconds) {
    struct timeval tv;
    tv.tv_sec = timeout_seconds;
    tv.tv_usec = 0;
    
    if (setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
        throw rpc_exception("Failed to set socket receive timeout");
    }
    
    if (setsockopt(socket_fd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv)) < 0) {
        throw rpc_exception("Failed to set socket send timeout");
    }
}

// 设置socket非阻塞
void set_socket_nonblocking(int socket_fd) {
    int flags = fcntl(socket_fd, F_GETFL, 0);
    if (flags < 0) {
        throw rpc_exception("Failed to get socket flags");
    }
    
    if (fcntl(socket_fd, F_SETFL, flags | O_NONBLOCK) < 0) {
        throw rpc_exception("Failed to set socket non-blocking");
    }
}

// 设置socket重用地址
void set_socket_reuseaddr(int socket_fd) {
    int opt = 1;
    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        throw rpc_exception("Failed to set socket reuse address");
    }
}

// 检查socket连接状态
bool is_socket_connected(int socket_fd) {
    int error = 0;
    socklen_t len = sizeof(error);
    
    if (getsockopt(socket_fd, SOL_SOCKET, SO_ERROR, &error, &len) < 0) {
        return false;
    }
    
    return error == 0;
}

// 关闭socket
void close_socket(int socket_fd) {
    if (socket_fd >= 0) {
        close(socket_fd);
    }
}

} // namespace network_utils

} // namespace rpc
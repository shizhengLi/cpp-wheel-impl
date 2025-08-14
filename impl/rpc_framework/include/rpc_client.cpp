#include "rpc_framework.hpp"
#include <iostream>
#include <sstream>
#include <chrono>
#include <thread>

namespace rpc {

RpcClient::RpcClient(const std::string& server_ip, uint16_t server_port)
    : server_ip_(server_ip)
    , server_port_(server_port)
    , socket_fd_(-1)
    , connected_(false)
    , running_(false)
    , next_message_id_(1) {
}

RpcClient::~RpcClient() {
    stop_heartbeat();
    disconnect();
}

void RpcClient::connect() {
    if (connected_) {
        return;
    }
    
    // 创建socket
    socket_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd_ < 0) {
        throw rpc_exception("Failed to create socket");
    }
    
    // 设置服务器地址
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port_);
    
    if (inet_pton(AF_INET, server_ip_.c_str(), &server_addr.sin_addr) <= 0) {
        close(socket_fd_);
        throw rpc_exception("Invalid server address");
    }
    
    // 连接服务器
    if (::connect(socket_fd_, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        close(socket_fd_);
        throw rpc_exception("Failed to connect to server");
    }
    
    connected_ = true;
    
    // 启动响应处理线程
    running_ = true;
    std::thread response_thread(&RpcClient::handle_responses, this);
    response_thread.detach();
}

void RpcClient::disconnect() {
    if (!connected_) {
        return;
    }
    
    running_ = false;
    connected_ = false;
    
    if (socket_fd_ >= 0) {
        close(socket_fd_);
        socket_fd_ = -1;
    }
}

bool RpcClient::is_connected() const {
    return connected_;
}

void RpcClient::send_message(const Message& message) {
    std::lock_guard<std::mutex> lock(socket_mutex_);
    
    if (!connected_) {
        throw rpc_exception("Not connected to server");
    }
    
    // 序列化消息
    std::string serialized_message = serialize_message(message);
    
    // 发送消息
    ssize_t bytes_sent = send(socket_fd_, serialized_message.c_str(), serialized_message.size(), 0);
    if (bytes_sent < 0) {
        throw rpc_exception("Failed to send message");
    }
    
    if (static_cast<size_t>(bytes_sent) != serialized_message.size()) {
        throw rpc_exception("Failed to send complete message");
    }
}

Message RpcClient::receive_message() {
    std::lock_guard<std::mutex> lock(socket_mutex_);
    
    if (!connected_) {
        throw rpc_exception("Not connected to server");
    }
    
    // 读取消息头
    char header_buffer[28];
    ssize_t bytes_received = recv(socket_fd_, header_buffer, 28, MSG_WAITALL);
    
    if (bytes_received < 0) {
        throw rpc_exception("Failed to receive message header");
    }
    
    if (bytes_received == 0) {
        connected_ = false;
        throw rpc_exception("Connection closed by server");
    }
    
    if (bytes_received != 28) {
        throw rpc_exception("Incomplete message header received");
    }
    
    // 反序列化消息头
    std::string header_data(header_buffer, 28);
    MessageHeader header = deserialize_header(header_data);
    
    if (!validate_header(header)) {
        throw rpc_exception("Invalid message header");
    }
    
    // 读取消息负载
    std::string payload;
    if (header.payload_size > 0) {
        payload.resize(header.payload_size);
        bytes_received = recv(socket_fd_, &payload[0], header.payload_size, MSG_WAITALL);
        
        if (bytes_received < 0) {
            throw rpc_exception("Failed to receive message payload");
        }
        
        if (static_cast<size_t>(bytes_received) != header.payload_size) {
            throw rpc_exception("Incomplete message payload received");
        }
    }
    
    // 构造消息
    Message message;
    message.header = header;
    message.payload = payload;
    
    return message;
}

void RpcClient::handle_responses() {
    while (running_) {
        try {
            Message response = receive_message();
            
            // 处理响应
            if (response.header.message_type == static_cast<uint32_t>(MessageType::RESPONSE) ||
                response.header.message_type == static_cast<uint32_t>(MessageType::ERROR)) {
                
                std::lock_guard<std::mutex> lock(pending_mutex_);
                auto it = pending_calls_.find(response.header.message_id);
                if (it != pending_calls_.end()) {
                    if (response.header.message_type == static_cast<uint32_t>(MessageType::ERROR)) {
                        it->second.set_value("ERR:" + response.payload);
                    } else {
                        it->second.set_value(response.payload);
                    }
                    pending_calls_.erase(it);
                }
            }
            
        } catch (const std::exception& e) {
            std::cerr << "Error handling response: " << e.what() << std::endl;
            connected_ = false;
            break;
        }
    }
}

void RpcClient::start_heartbeat() {
    if (running_) {
        return;
    }
    
    running_ = true;
    heartbeat_thread_ = std::thread(&RpcClient::heartbeat_loop, this);
}

void RpcClient::stop_heartbeat() {
    if (!running_) {
        return;
    }
    
    running_ = false;
    if (heartbeat_thread_.joinable()) {
        heartbeat_thread_.join();
    }
}

void RpcClient::heartbeat_loop() {
    while (running_ && connected_) {
        try {
            // 发送心跳消息
            Message heartbeat = create_heartbeat_message(next_message_id_++);
            send_message(heartbeat);
            
            // 等待5秒
            std::this_thread::sleep_for(std::chrono::seconds(5));
            
        } catch (const std::exception& e) {
            std::cerr << "Heartbeat failed: " << e.what() << std::endl;
            connected_ = false;
            break;
        }
    }
}

// 工厂函数
std::shared_ptr<RpcClient> create_rpc_client(const std::string& server_ip, uint16_t server_port) {
    return std::make_shared<RpcClient>(server_ip, server_port);
}

} // namespace rpc
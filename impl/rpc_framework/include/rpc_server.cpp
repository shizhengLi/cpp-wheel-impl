#include "rpc_framework.hpp"
#include <iostream>
#include <sstream>
#include <thread>
#include <vector>
#include <algorithm>

namespace rpc {

// 声明network_utils命名空间
namespace network_utils {
    void set_socket_reuseaddr(int socket_fd);
    void close_socket(int socket_fd);
}

RpcServer::RpcServer(uint16_t port)
    : port_(port)
    , server_fd_(-1)
    , running_(false)
    , total_calls_(0)
    , failed_calls_(0) {
}

RpcServer::~RpcServer() {
    stop();
}

void RpcServer::register_service(std::shared_ptr<Service> service) {
    if (!service) {
        throw rpc_exception("Invalid service pointer");
    }
    
    std::lock_guard<std::mutex> lock(services_mutex_);
    uint32_t service_id = service->get_service_id();
    
    if (services_.find(service_id) != services_.end()) {
        throw rpc_exception("Service ID already registered: " + std::to_string(service_id));
    }
    
    services_[service_id] = service;
    std::cout << "Service registered: " << service->get_service_name() 
              << " (ID: " << service_id << ")" << std::endl;
}

void RpcServer::unregister_service(uint32_t service_id) {
    std::lock_guard<std::mutex> lock(services_mutex_);
    
    auto it = services_.find(service_id);
    if (it != services_.end()) {
        std::cout << "Service unregistered: " << it->second->get_service_name() 
                  << " (ID: " << service_id << ")" << std::endl;
        services_.erase(it);
    }
}

void RpcServer::start() {
    if (running_) {
        return;
    }
    
    // 创建服务器socket
    server_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd_ < 0) {
        throw rpc_exception("Failed to create server socket");
    }
    
    // 设置socket选项
    network_utils::set_socket_reuseaddr(server_fd_);
    
    // 绑定地址
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port_);
    
    if (bind(server_fd_, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        close(server_fd_);
        throw rpc_exception("Failed to bind server socket");
    }
    
    // 开始监听
    if (listen(server_fd_, 128) < 0) {
        close(server_fd_);
        throw rpc_exception("Failed to listen on server socket");
    }
    
    running_ = true;
    std::cout << "RPC Server started on port " << port_ << std::endl;
    
    // 启动接受连接线程
    std::thread accept_thread(&RpcServer::accept_connections, this);
    accept_thread.detach();
}

void RpcServer::stop() {
    if (!running_) {
        return;
    }
    
    running_ = false;
    
    // 关闭服务器socket
    if (server_fd_ >= 0) {
        close(server_fd_);
        server_fd_ = -1;
    }
    
    // 等待工作线程结束
    for (auto& thread : worker_threads_) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    worker_threads_.clear();
    
    std::cout << "RPC Server stopped" << std::endl;
}

bool RpcServer::is_running() const {
    return running_;
}

void RpcServer::accept_connections() {
    while (running_) {
        try {
            struct sockaddr_in client_addr;
            socklen_t client_len = sizeof(client_addr);
            
            int client_fd = accept(server_fd_, (struct sockaddr*)&client_addr, &client_len);
            if (client_fd < 0) {
                if (running_) {
                    std::cerr << "Failed to accept client connection" << std::endl;
                }
                continue;
            }
            
            // 启动客户端处理线程
            worker_threads_.emplace_back(&RpcServer::handle_client, this, client_fd);
            
            // 分离线程，让它独立运行
            worker_threads_.back().detach();
            
            // 清理已完成的线程
            worker_threads_.erase(
                std::remove_if(worker_threads_.begin(), worker_threads_.end(),
                    [](const std::thread& t) { return !t.joinable(); }),
                worker_threads_.end()
            );
            
        } catch (const std::exception& e) {
            std::cerr << "Error accepting connection: " << e.what() << std::endl;
        }
    }
}

void RpcServer::handle_client(int client_fd) {
    try {
        while (running_) {
            // 接收消息
            Message request = receive_message(client_fd);
            
            // 处理请求
            Message response = process_request(request);
            
            // 发送响应
            send_message(client_fd, response);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error handling client: " << e.what() << std::endl;
    }
    
    // 关闭客户端连接
    close(client_fd);
}

Message RpcServer::receive_message(int client_fd) {
    // 读取消息头
    char header_buffer[28];
    ssize_t bytes_received = recv(client_fd, header_buffer, 28, MSG_WAITALL);
    
    if (bytes_received < 0) {
        throw rpc_exception("Failed to receive message header");
    }
    
    if (bytes_received == 0) {
        throw rpc_exception("Client disconnected");
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
        bytes_received = recv(client_fd, &payload[0], header.payload_size, MSG_WAITALL);
        
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

void RpcServer::send_message(int client_fd, const Message& message) {
    // 序列化消息
    std::string serialized_message = serialize_message(message);
    
    // 发送消息
    ssize_t bytes_sent = send(client_fd, serialized_message.c_str(), serialized_message.size(), 0);
    if (bytes_sent < 0) {
        throw rpc_exception("Failed to send message");
    }
    
    if (static_cast<size_t>(bytes_sent) != serialized_message.size()) {
        throw rpc_exception("Failed to send complete message");
    }
}

Message RpcServer::process_request(const Message& request) {
    total_calls_++;
    
    try {
        // 检查消息类型
        if (request.header.message_type != static_cast<uint32_t>(MessageType::REQUEST)) {
            throw rpc_exception("Invalid message type");
        }
        
        // 查找服务
        std::shared_ptr<Service> service;
        {
            std::lock_guard<std::mutex> lock(services_mutex_);
            auto it = services_.find(request.header.service_id);
            if (it == services_.end()) {
                throw rpc_exception("Service not found: " + std::to_string(request.header.service_id));
            }
            service = it->second;
        }
        
        // 调用服务方法
        std::string result = service->call_method(request.header.method_id, request.payload);
        
        // 创建响应消息
        return create_response_message(
            request.header.service_id,
            request.header.method_id,
            request.header.message_id,
            result
        );
        
    } catch (const std::exception& e) {
        failed_calls_++;
        
        // 创建错误消息
        return create_error_message(
            request.header.service_id,
            request.header.method_id,
            request.header.message_id,
            e.what()
        );
    }
}

std::string RpcServer::get_stats() const {
    std::stringstream ss;
    ss << "RPC Server Stats:\n"
       << "  Port: " << port_ << "\n"
       << "  Running: " << (running_ ? "Yes" : "No") << "\n"
       << "  Services: " << services_.size() << "\n"
       << "  Total Calls: " << total_calls_.load() << "\n"
       << "  Failed Calls: " << failed_calls_.load() << "\n"
       << "  Success Rate: " 
       << (total_calls_.load() > 0 ? 
           (100.0 * (total_calls_.load() - failed_calls_.load()) / total_calls_.load()) : 100.0)
       << "%";
    
    return ss.str();
}

// 工厂函数
std::shared_ptr<RpcServer> create_rpc_server(uint16_t port) {
    return std::make_shared<RpcServer>(port);
}

} // namespace rpc
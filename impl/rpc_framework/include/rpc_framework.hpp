#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <future>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <stdexcept>
#include <cstdint>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

namespace rpc {

/**
 * @brief RPC框架异常类
 */
class rpc_exception : public std::runtime_error {
public:
    explicit rpc_exception(const std::string& msg) : std::runtime_error(msg) {}
};

/**
 * @brief 序列化接口
 */
class Serializer {
public:
    virtual ~Serializer() = default;
    virtual void serialize(const std::string& data) = 0;
    virtual std::string deserialize() = 0;
    virtual void reset() = 0;
};

/**
 * @brief RPC消息类型
 */
enum class MessageType {
    REQUEST = 1,
    RESPONSE = 2,
    ERROR = 3,
    HEARTBEAT = 4
};

/**
 * @brief RPC消息头
 */
struct MessageHeader {
    uint32_t magic_number;    // 魔数 0xRPCF
    uint32_t message_id;      // 消息ID
    uint32_t message_type;   // 消息类型
    uint32_t service_id;     // 服务ID
    uint32_t method_id;      // 方法ID
    uint32_t payload_size;   // 负载大小
    uint32_t sequence_id;    // 序列号
};

/**
 * @brief RPC消息
 */
struct Message {
    MessageHeader header;
    std::string payload;
};

/**
 * @brief RPC服务接口
 */
class Service {
public:
    virtual ~Service() = default;
    virtual std::string call_method(uint32_t method_id, const std::string& args) = 0;
    virtual uint32_t get_service_id() const = 0;
    virtual std::string get_service_name() const = 0;
};

/**
 * @brief RPC客户端
 */
class RpcClient {
public:
    RpcClient(const std::string& server_ip, uint16_t server_port);
    ~RpcClient();
    
    // 禁用拷贝
    RpcClient(const RpcClient&) = delete;
    RpcClient& operator=(const RpcClient&) = delete;
    
    // 连接管理
    void connect();
    void disconnect();
    bool is_connected() const;
    
    // RPC调用
    template<typename Ret, typename... Args>
    Ret call(uint32_t service_id, uint32_t method_id, const Args&... args);
    
    // 异步RPC调用
    template<typename Ret, typename... Args>
    std::future<Ret> async_call(uint32_t service_id, uint32_t method_id, const Args&... args);
    
    // 心跳检测
    void start_heartbeat();
    void stop_heartbeat();
    
private:
    std::string server_ip_;
    uint16_t server_port_;
    int socket_fd_;
    std::atomic<bool> connected_;
    std::atomic<bool> running_;
    std::thread heartbeat_thread_;
    std::mutex socket_mutex_;
    std::map<uint32_t, std::promise<std::string>> pending_calls_;
    std::mutex pending_mutex_;
    std::condition_variable pending_cv_;
    std::atomic<uint32_t> next_message_id_;
    
    // 网络操作
    void send_message(const Message& message);
    Message receive_message();
    void handle_responses();
    void heartbeat_loop();
    
    // 序列化
    template<typename... Args>
    std::string serialize_args(const Args&... args);
    
    template<typename Ret>
    Ret deserialize_result(const std::string& data);
};

/**
 * @brief RPC服务器
 */
class RpcServer {
public:
    RpcServer(uint16_t port);
    ~RpcServer();
    
    // 禁用拷贝
    RpcServer(const RpcServer&) = delete;
    RpcServer& operator=(const RpcServer&) = delete;
    
    // 服务管理
    void register_service(std::shared_ptr<Service> service);
    void unregister_service(uint32_t service_id);
    
    // 服务器控制
    void start();
    void stop();
    bool is_running() const;
    
    // 获取统计信息
    std::string get_stats() const;
    
private:
    uint16_t port_;
    int server_fd_;
    std::atomic<bool> running_;
    std::map<uint32_t, std::shared_ptr<Service>> services_;
    std::mutex services_mutex_;
    std::vector<std::thread> worker_threads_;
    std::atomic<uint64_t> total_calls_;
    std::atomic<uint64_t> failed_calls_;
    
    // 网络操作
    void accept_connections();
    void handle_client(int client_fd);
    Message receive_message(int client_fd);
    void send_message(int client_fd, const Message& message);
    
    // RPC处理
    Message process_request(const Message& request);
};

/**
 * @brief 服务注册中心
 */
class ServiceRegistry {
public:
    static ServiceRegistry& get_instance();
    
    // 服务注册
    void register_service(const std::string& service_name, const std::string& server_address, uint16_t port);
    void unregister_service(const std::string& service_name, const std::string& server_address);
    
    // 服务发现
    std::vector<std::pair<std::string, uint16_t>> discover_service(const std::string& service_name);
    
    // 健康检查
    void start_health_check();
    void stop_health_check();
    
private:
    ServiceRegistry() = default;
    ~ServiceRegistry() = default;
    
    std::map<std::string, std::vector<std::pair<std::string, uint16_t>>> services_;
    std::mutex registry_mutex_;
    std::atomic<bool> health_check_running_;
    std::thread health_check_thread_;
    
    void health_check_loop();
    bool is_service_alive(const std::string& address, uint16_t port);
};

/**
 * @brief 负载均衡器
 */
class LoadBalancer {
public:
    enum class Strategy {
        ROUND_ROBIN,
        RANDOM,
        LEAST_CONNECTIONS
    };
    
    LoadBalancer(Strategy strategy = Strategy::ROUND_ROBIN);
    
    // 添加服务器
    void add_server(const std::string& address, uint16_t port);
    void remove_server(const std::string& address, uint16_t port);
    
    // 选择服务器
    std::pair<std::string, uint16_t> select_server();
    
    // 更新连接数
    void update_connections(const std::string& address, uint16_t port, int delta);
    
private:
    Strategy strategy_;
    std::vector<std::pair<std::string, uint16_t>> servers_;
    std::map<std::string, int> connections_;
    std::mutex balancer_mutex_;
    std::atomic<size_t> round_robin_index_;
    
    std::pair<std::string, uint16_t> select_round_robin();
    std::pair<std::string, uint16_t> select_random();
    std::pair<std::string, uint16_t> select_least_connections();
};

/**
 * @brief 工厂函数：创建RPC客户端
 */
std::shared_ptr<RpcClient> create_rpc_client(const std::string& server_ip, uint16_t server_port);

/**
 * @brief 工厂函数：创建RPC服务器
 */
std::shared_ptr<RpcServer> create_rpc_server(uint16_t port);

// 协议函数
std::string serialize_header(const MessageHeader& header);
MessageHeader deserialize_header(const std::string& data);
std::string serialize_message(const Message& message);
Message deserialize_message(const std::string& data);
Message create_request_message(uint32_t service_id, uint32_t method_id, 
                             uint32_t message_id, const std::string& payload);
Message create_response_message(uint32_t service_id, uint32_t method_id,
                              uint32_t message_id, const std::string& payload);
Message create_error_message(uint32_t service_id, uint32_t method_id,
                           uint32_t message_id, const std::string& error_msg);
Message create_heartbeat_message(uint32_t message_id);
uint32_t generate_message_id();
bool validate_header(const MessageHeader& header);
std::string get_message_type_string(MessageType type);

} // namespace rpc

// 模板实现
#include "rpc_client.tpp"
#include "rpc_serializer.tpp"
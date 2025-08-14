#include "rpc_framework.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <random>

namespace rpc {

// ServiceRegistry 实现
ServiceRegistry& ServiceRegistry::get_instance() {
    static ServiceRegistry instance;
    return instance;
}

void ServiceRegistry::register_service(const std::string& service_name, 
                                     const std::string& server_address, uint16_t port) {
    std::lock_guard<std::mutex> lock(registry_mutex_);
    
    auto& instances = services_[service_name];
    
    // 检查是否已经注册
    for (const auto& instance : instances) {
        if (instance.first == server_address && instance.second == port) {
            return; // 已经注册
        }
    }
    
    instances.emplace_back(server_address, port);
    std::cout << "Service registered: " << service_name << " at " 
              << server_address << ":" << port << std::endl;
}

void ServiceRegistry::unregister_service(const std::string& service_name, 
                                       const std::string& server_address) {
    std::lock_guard<std::mutex> lock(registry_mutex_);
    
    auto it = services_.find(service_name);
    if (it != services_.end()) {
        auto& instances = it->second;
        instances.erase(
            std::remove_if(instances.begin(), instances.end(),
                [&server_address](const auto& instance) {
                    return instance.first == server_address;
                }),
            instances.end()
        );
        
        if (instances.empty()) {
            services_.erase(it);
        }
        
        std::cout << "Service unregistered: " << service_name << " from " 
                  << server_address << std::endl;
    }
}

std::vector<std::pair<std::string, uint16_t>> ServiceRegistry::discover_service(
    const std::string& service_name) {
    std::lock_guard<std::mutex> lock(registry_mutex_);
    
    auto it = services_.find(service_name);
    if (it == services_.end()) {
        return {};
    }
    
    return it->second;
}

void ServiceRegistry::start_health_check() {
    if (health_check_running_) {
        return;
    }
    
    health_check_running_ = true;
    health_check_thread_ = std::thread(&ServiceRegistry::health_check_loop, this);
}

void ServiceRegistry::stop_health_check() {
    if (!health_check_running_) {
        return;
    }
    
    health_check_running_ = false;
    if (health_check_thread_.joinable()) {
        health_check_thread_.join();
    }
}

void ServiceRegistry::health_check_loop() {
    while (health_check_running_) {
        std::vector<std::pair<std::string, std::string>> dead_services;
        
        {
            std::lock_guard<std::mutex> lock(registry_mutex_);
            
            for (auto& [service_name, instances] : services_) {
                for (auto it = instances.begin(); it != instances.end(); ) {
                    if (!is_service_alive(it->first, it->second)) {
                        dead_services.emplace_back(service_name, it->first);
                        it = instances.erase(it);
                    } else {
                        ++it;
                    }
                }
                
                if (instances.empty()) {
                    services_.erase(service_name);
                }
            }
        }
        
        // 输出死掉的服务信息
        for (const auto& [service_name, address] : dead_services) {
            std::cout << "Service died: " << service_name << " at " << address << std::endl;
        }
        
        // 等待30秒
        std::this_thread::sleep_for(std::chrono::seconds(30));
    }
}

bool ServiceRegistry::is_service_alive(const std::string& address, uint16_t port) {
    try {
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) {
            return false;
        }
        
        // 设置超时
        struct timeval tv;
        tv.tv_sec = 5;
        tv.tv_usec = 0;
        setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
        setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
        
        struct sockaddr_in server_addr;
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port);
        
        if (inet_pton(AF_INET, address.c_str(), &server_addr.sin_addr) <= 0) {
            close(sock);
            return false;
        }
        
        int result = connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
        close(sock);
        
        return result == 0;
    } catch (const std::exception& e) {
        return false;
    }
}

// LoadBalancer 实现
LoadBalancer::LoadBalancer(Strategy strategy) 
    : strategy_(strategy), round_robin_index_(0) {
}

void LoadBalancer::add_server(const std::string& address, uint16_t port) {
    std::lock_guard<std::mutex> lock(balancer_mutex_);
    
    // 检查是否已经存在
    for (const auto& server : servers_) {
        if (server.first == address && server.second == port) {
            return;
        }
    }
    
    servers_.emplace_back(address, port);
    connections_[address + ":" + std::to_string(port)] = 0;
    
    std::cout << "Load balancer added server: " << address << ":" << port << std::endl;
}

void LoadBalancer::remove_server(const std::string& address, uint16_t port) {
    std::lock_guard<std::mutex> lock(balancer_mutex_);
    
    servers_.erase(
        std::remove_if(servers_.begin(), servers_.end(),
            [&address, port](const auto& server) {
                return server.first == address && server.second == port;
            }),
        servers_.end()
    );
    
    connections_.erase(address + ":" + std::to_string(port));
    
    std::cout << "Load balancer removed server: " << address << ":" << port << std::endl;
}

std::pair<std::string, uint16_t> LoadBalancer::select_server() {
    std::lock_guard<std::mutex> lock(balancer_mutex_);
    
    if (servers_.empty()) {
        throw rpc_exception("No servers available");
    }
    
    switch (strategy_) {
        case Strategy::ROUND_ROBIN:
            return select_round_robin();
        case Strategy::RANDOM:
            return select_random();
        case Strategy::LEAST_CONNECTIONS:
            return select_least_connections();
        default:
            return select_round_robin();
    }
}

void LoadBalancer::update_connections(const std::string& address, uint16_t port, int delta) {
    std::lock_guard<std::mutex> lock(balancer_mutex_);
    
    std::string key = address + ":" + std::to_string(port);
    connections_[key] += delta;
    
    if (connections_[key] < 0) {
        connections_[key] = 0;
    }
}

std::pair<std::string, uint16_t> LoadBalancer::select_round_robin() {
    if (servers_.empty()) {
        throw rpc_exception("No servers available");
    }
    
    size_t index = round_robin_index_++ % servers_.size();
    return servers_[index];
}

std::pair<std::string, uint16_t> LoadBalancer::select_random() {
    if (servers_.empty()) {
        throw rpc_exception("No servers available");
    }
    
    static std::random_device rd;
    static std::mt19937 gen(rd());
    
    std::uniform_int_distribution<size_t> dist(0, servers_.size() - 1);
    size_t index = dist(gen);
    
    return servers_[index];
}

std::pair<std::string, uint16_t> LoadBalancer::select_least_connections() {
    if (servers_.empty()) {
        throw rpc_exception("No servers available");
    }
    
    std::pair<std::string, uint16_t> best_server = servers_[0];
    int min_connections = connections_[best_server.first + ":" + std::to_string(best_server.second)];
    
    for (const auto& server : servers_) {
        std::string key = server.first + ":" + std::to_string(server.second);
        int connections = connections_[key];
        
        if (connections < min_connections) {
            min_connections = connections;
            best_server = server;
        }
    }
    
    return best_server;
}

} // namespace rpc
#include <gtest/gtest.h>
#include <thread>
#include <chrono>
#include <atomic>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include "rpc_framework.hpp"

using namespace rpc;

// 测试服务实现
class TestService : public Service {
private:
    uint32_t service_id_;
    std::string service_name_;
    
public:
    TestService(uint32_t id, const std::string& name) 
        : service_id_(id), service_name_(name) {}
    
    std::string call_method(uint32_t method_id, const std::string& args) override {
        switch (method_id) {
            case 1: // add
                return add_method(args);
            case 2: // concatenate
                return concatenate_method(args);
            case 3: // get_info
                return get_info_method(args);
            case 4: // echo
                return echo_method(args);
            default:
                throw rpc_exception("Unknown method: " + std::to_string(method_id));
        }
    }
    
    uint32_t get_service_id() const override {
        return service_id_;
    }
    
    std::string get_service_name() const override {
        return service_name_;
    }
    
private:
    std::string add_method(const std::string& args) {
        // 解析参数 "a,b"
        size_t comma_pos = args.find(',');
        if (comma_pos == std::string::npos) {
            throw rpc_exception("Invalid arguments for add method");
        }
        
        int a = std::stoi(args.substr(0, comma_pos));
        int b = std::stoi(args.substr(comma_pos + 1));
        
        return std::to_string(a + b);
    }
    
    std::string concatenate_method(const std::string& args) {
        // 解析参数 "str1,str2"
        size_t comma_pos = args.find(',');
        if (comma_pos == std::string::npos) {
            throw rpc_exception("Invalid arguments for concatenate method");
        }
        
        std::string str1 = args.substr(0, comma_pos);
        std::string str2 = args.substr(comma_pos + 1);
        
        return str1 + str2;
    }
    
    std::string get_info_method(const std::string& args) {
        (void)args; // 避免未使用参数警告
        return "TestService-" + std::to_string(service_id_) + ":" + service_name_;
    }
    
    std::string echo_method(const std::string& args) {
        return "Echo: " + args;
    }
};

class RpcFrameworkTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 使用固定端口
        server_port = 18080;
        
        // 启动RPC服务器
        server = create_rpc_server(server_port);
        server->register_service(std::make_shared<TestService>(1, "TestService1"));
        server->register_service(std::make_shared<TestService>(2, "TestService2"));
        server->start();
        
        // 等待服务器启动
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        // 创建RPC客户端
        client = create_rpc_client("127.0.0.1", server_port);
        client->connect();
    }
    
    void TearDown() override {
        if (client) {
            client->disconnect();
        }
        if (server) {
            server->stop();
        }
    }
    
    std::shared_ptr<RpcServer> server;
    std::shared_ptr<RpcClient> client;
    uint16_t server_port;
};

// 基本RPC调用测试
TEST_F(RpcFrameworkTest, BasicRpcCall) {
    // 测试加法
    int result = client->call<int>(1, 1, "5,3");
    EXPECT_EQ(result, 8);
    
    // 测试字符串连接
    std::string concat_result = client->call<std::string>(1, 2, "Hello,World");
    EXPECT_EQ(concat_result, "HelloWorld");
    
    // 测试echo
    std::string echo_result = client->call<std::string>(1, 4, "Test Message");
    EXPECT_EQ(echo_result, "Echo: Test Message");
}

// 异步RPC调用测试
TEST_F(RpcFrameworkTest, AsyncRpcCall) {
    // 异步加法调用
    auto future1 = client->async_call<int>(1, 1, "10,20");
    auto future2 = client->async_call<std::string>(1, 2, "Async,Test");
    
    // 等待结果
    int result1 = future1.get();
    std::string result2 = future2.get();
    
    EXPECT_EQ(result1, 30);
    EXPECT_EQ(result2, "AsyncTest");
}

// 多服务测试
TEST_F(RpcFrameworkTest, MultipleServices) {
    // 调用服务1
    std::string info1 = client->call<std::string>(1, 3, "");
    EXPECT_EQ(info1, "TestService-1:TestService1");
    
    // 调用服务2
    std::string info2 = client->call<std::string>(2, 3, "");
    EXPECT_EQ(info2, "TestService-2:TestService2");
}

// 错误处理测试
TEST_F(RpcFrameworkTest, ErrorHandling) {
    // 测试未知服务
    EXPECT_THROW(client->call<std::string>(999, 1, "test"), rpc_exception);
    
    // 测试未知方法
    EXPECT_THROW(client->call<std::string>(1, 999, "test"), rpc_exception);
    
    // 测试无效参数
    EXPECT_THROW(client->call<int>(1, 1, "invalid"), rpc_exception);
}

// 序列化测试
TEST_F(RpcFrameworkTest, Serialization) {
    // 测试消息序列化
    MessageHeader header;
    header.magic_number = 0x52504346;
    header.message_id = 12345;
    header.message_type = static_cast<uint32_t>(MessageType::REQUEST);
    header.service_id = 1;
    header.method_id = 2;
    header.payload_size = 10;
    header.sequence_id = 0;
    
    // 创建消息并序列化
    Message message;
    message.header = header;
    message.payload = "test payload";
    
    std::string serialized = serialize_message(message);
    Message deserialized = deserialize_message(serialized);
    
    EXPECT_EQ(deserialized.header.magic_number, header.magic_number);
    EXPECT_EQ(deserialized.header.message_id, header.message_id);
    EXPECT_EQ(deserialized.header.message_type, header.message_type);
    EXPECT_EQ(deserialized.header.service_id, header.service_id);
    EXPECT_EQ(deserialized.header.method_id, header.method_id);
    EXPECT_EQ(deserialized.header.payload_size, header.payload_size);
    EXPECT_EQ(deserialized.header.sequence_id, header.sequence_id);
}

// 服务注册中心测试
TEST_F(RpcFrameworkTest, ServiceRegistry) {
    auto& registry = ServiceRegistry::get_instance();
    
    // 注册服务
    registry.register_service("TestService", "127.0.0.1", 8080);
    registry.register_service("TestService", "127.0.0.1", 8081);
    registry.register_service("AnotherService", "127.0.0.1", 8090);
    
    // 发现服务
    auto instances = registry.discover_service("TestService");
    EXPECT_EQ(instances.size(), 2);
    
    instances = registry.discover_service("AnotherService");
    EXPECT_EQ(instances.size(), 1);
    
    instances = registry.discover_service("NonExistentService");
    EXPECT_EQ(instances.size(), 0);
    
    // 注销服务
    registry.unregister_service("TestService", "127.0.0.1");
    
    instances = registry.discover_service("TestService");
    EXPECT_EQ(instances.size(), 1);
}

// 负载均衡测试
TEST_F(RpcFrameworkTest, LoadBalancing) {
    LoadBalancer balancer(LoadBalancer::Strategy::ROUND_ROBIN);
    
    // 添加服务器
    balancer.add_server("127.0.0.1", 8080);
    balancer.add_server("127.0.0.1", 8081);
    balancer.add_server("127.0.0.1", 8082);
    
    // 测试轮询
    auto server1 = balancer.select_server();
    auto server2 = balancer.select_server();
    auto server3 = balancer.select_server();
    auto server4 = balancer.select_server();
    
    EXPECT_EQ(server4, server1); // 循环回到第一个
    
    // 测试随机策略
    LoadBalancer random_balancer(LoadBalancer::Strategy::RANDOM);
    random_balancer.add_server("127.0.0.1", 8080);
    random_balancer.add_server("127.0.0.1", 8081);
    
    auto random_server = random_balancer.select_server();
    EXPECT_TRUE(random_server.first == "127.0.0.1");
    EXPECT_TRUE(random_server.second == 8080 || random_server.second == 8081);
    
    // 测试最少连接策略
    LoadBalancer lc_balancer(LoadBalancer::Strategy::LEAST_CONNECTIONS);
    lc_balancer.add_server("127.0.0.1", 8080);
    lc_balancer.add_server("127.0.0.1", 8081);
    
    lc_balancer.update_connections("127.0.0.1", 8080, 5);
    lc_balancer.update_connections("127.0.0.1", 8081, 2);
    
    auto lc_server = lc_balancer.select_server();
    EXPECT_EQ(lc_server.second, 8081); // 应该选择连接数较少的服务器
}

// 并发测试
TEST_F(RpcFrameworkTest, Concurrency) {
    const int num_threads = 10;
    const int num_calls = 100;
    std::atomic<int> success_count{0};
    std::atomic<int> error_count{0};
    
    std::vector<std::thread> threads;
    
    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back([this, num_calls, &success_count, &error_count]() {
            for (int j = 0; j < num_calls; ++j) {
                try {
                    int result = client->call<int>(1, 1, std::to_string(j) + "," + std::to_string(j));
                    EXPECT_EQ(result, j * 2);
                    success_count++;
                } catch (const std::exception& e) {
                    error_count++;
                }
            }
        });
    }
    
    // 等待所有线程完成
    for (auto& thread : threads) {
        thread.join();
    }
    
    EXPECT_EQ(success_count + error_count, num_threads * num_calls);
    EXPECT_GT(success_count, 0);
}

// 性能测试
TEST_F(RpcFrameworkTest, Performance) {
    const int num_calls = 1000;
    auto start_time = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < num_calls; ++i) {
        int result = client->call<int>(1, 1, "1,1");
        EXPECT_EQ(result, 2);
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    
    std::cout << "Performance test: " << num_calls << " calls in " 
              << duration.count() << " ms" << std::endl;
    std::cout << "Average: " << (duration.count() * 1000.0 / num_calls) << " μs per call" << std::endl;
    
    EXPECT_LT(duration.count(), 10000); // 应该在10秒内完成
}

// 心跳测试
TEST_F(RpcFrameworkTest, Heartbeat) {
    // 启动心跳
    client->start_heartbeat();
    
    // 等待一段时间
    std::this_thread::sleep_for(std::chrono::seconds(2));
    
    // 验证连接仍然活跃
    EXPECT_TRUE(client->is_connected());
    
    // 停止心跳
    client->stop_heartbeat();
}

// 服务器统计测试
TEST_F(RpcFrameworkTest, ServerStats) {
    // 执行一些调用
    for (int i = 0; i < 10; ++i) {
        client->call<int>(1, 1, "1,1");
    }
    
    // 获取统计信息
    std::string stats = server->get_stats();
    
    EXPECT_NE(stats.find("Total Calls:"), std::string::npos);
    EXPECT_NE(stats.find("Failed Calls:"), std::string::npos);
    EXPECT_NE(stats.find("Success Rate:"), std::string::npos);
    
    std::cout << "Server stats:\n" << stats << std::endl;
}

// 序列化工具测试
TEST_F(RpcFrameworkTest, SerializationUtils) {
    // 测试基本类型序列化
    std::string int_str = SerializationUtils::serialize_basic(42);
    int int_result = SerializationUtils::deserialize_basic<int>(int_str);
    EXPECT_EQ(int_result, 42);
    
    std::string bool_str = SerializationUtils::serialize_basic(true);
    bool bool_result = SerializationUtils::deserialize_basic<bool>(bool_str);
    EXPECT_TRUE(bool_result);
    
    std::string double_str = SerializationUtils::serialize_basic(3.14);
    double double_result = SerializationUtils::deserialize_basic<double>(double_str);
    EXPECT_NEAR(double_result, 3.14, 0.001);
    
    // 测试向量序列化
    std::vector<int> vec = {1, 2, 3, 4, 5};
    std::string vec_str = SerializationUtils::serialize_vector(vec);
    std::vector<int> vec_result = SerializationUtils::deserialize_vector<int>(vec_str);
    EXPECT_EQ(vec_result, vec);
    
    // 测试映射序列化
    std::map<std::string, int> map = {{"one", 1}, {"two", 2}, {"three", 3}};
    std::string map_str = SerializationUtils::serialize_map(map);
    std::map<std::string, int> map_result = SerializationUtils::deserialize_map<std::string, int>(map_str);
    EXPECT_EQ(map_result, map);
}

// 超时测试
TEST_F(RpcFrameworkTest, Timeout) {
    // 创建一个新的客户端，设置较短的连接超时
    auto timeout_client = create_rpc_client("127.0.0.1", server_port);
    
    // 测试连接超时（连接到一个不存在的服务器）
    auto bad_client = create_rpc_client("127.0.0.1", 9999);
    EXPECT_THROW(bad_client->connect(), rpc_exception);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
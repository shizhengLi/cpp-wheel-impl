#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include "rpc_framework.hpp"

using namespace rpc;

// 简单的RPC测试，不涉及网络通信
class RpcFrameworkSimpleTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 测试设置
    }
    
    void TearDown() override {
        // 测试清理
    }
};

// 消息序列化测试
TEST_F(RpcFrameworkSimpleTest, MessageSerialization) {
    // 创建消息并序列化
    Message message;
    message.header.magic_number = 0x52504346;
    message.header.message_id = 12345;
    message.header.message_type = static_cast<uint32_t>(MessageType::REQUEST);
    message.header.service_id = 1;
    message.header.method_id = 2;
    message.header.payload_size = 12; // "test payload" 的长度
    message.header.sequence_id = 0;
    message.payload = "test payload";
    
    std::string serialized = serialize_message(message);
    Message deserialized = deserialize_message(serialized);
    
    EXPECT_EQ(deserialized.header.magic_number, message.header.magic_number);
    EXPECT_EQ(deserialized.header.message_id, message.header.message_id);
    EXPECT_EQ(deserialized.header.message_type, message.header.message_type);
    EXPECT_EQ(deserialized.header.service_id, message.header.service_id);
    EXPECT_EQ(deserialized.header.method_id, message.header.method_id);
    EXPECT_EQ(deserialized.header.payload_size, message.header.payload_size);
    EXPECT_EQ(deserialized.header.sequence_id, message.header.sequence_id);
    EXPECT_EQ(deserialized.payload, message.payload);
}

// 消息创建测试
TEST_F(RpcFrameworkSimpleTest, MessageCreation) {
    // 测试请求消息
    Message request = create_request_message(1, 2, 12345, "test payload");
    EXPECT_EQ(request.header.magic_number, 0x52504346);
    EXPECT_EQ(request.header.message_id, 12345);
    EXPECT_EQ(request.header.message_type, static_cast<uint32_t>(MessageType::REQUEST));
    EXPECT_EQ(request.header.service_id, 1);
    EXPECT_EQ(request.header.method_id, 2);
    EXPECT_EQ(request.header.payload_size, 12);
    EXPECT_EQ(request.payload, "test payload");
    
    // 测试响应消息
    Message response = create_response_message(1, 2, 12345, "response data");
    EXPECT_EQ(response.header.magic_number, 0x52504346);
    EXPECT_EQ(response.header.message_id, 12345);
    EXPECT_EQ(response.header.message_type, static_cast<uint32_t>(MessageType::RESPONSE));
    EXPECT_EQ(response.header.service_id, 1);
    EXPECT_EQ(response.header.method_id, 2);
    EXPECT_EQ(response.header.payload_size, 13);
    EXPECT_EQ(response.payload, "response data");
    
    // 测试错误消息
    Message error = create_error_message(1, 2, 12345, "error message");
    EXPECT_EQ(error.header.magic_number, 0x52504346);
    EXPECT_EQ(error.header.message_id, 12345);
    EXPECT_EQ(error.header.message_type, static_cast<uint32_t>(MessageType::ERROR));
    EXPECT_EQ(error.header.service_id, 1);
    EXPECT_EQ(error.header.method_id, 2);
    EXPECT_EQ(error.header.payload_size, 13);
    EXPECT_EQ(error.payload, "error message");
    
    // 测试心跳消息
    Message heartbeat = create_heartbeat_message(12345);
    EXPECT_EQ(heartbeat.header.magic_number, 0x52504346);
    EXPECT_EQ(heartbeat.header.message_id, 12345);
    EXPECT_EQ(heartbeat.header.message_type, static_cast<uint32_t>(MessageType::HEARTBEAT));
    EXPECT_EQ(heartbeat.header.service_id, 0);
    EXPECT_EQ(heartbeat.header.method_id, 0);
    EXPECT_EQ(heartbeat.header.payload_size, 0);
    EXPECT_EQ(heartbeat.payload, "");
}

// 消息验证测试
TEST_F(RpcFrameworkSimpleTest, MessageValidation) {
    // 测试有效消息头
    MessageHeader valid_header;
    valid_header.magic_number = 0x52504346;
    EXPECT_TRUE(validate_header(valid_header));
    
    // 测试无效消息头
    MessageHeader invalid_header;
    invalid_header.magic_number = 0x12345678;
    EXPECT_FALSE(validate_header(invalid_header));
}

// 序列化工具测试
TEST_F(RpcFrameworkSimpleTest, SerializationUtils) {
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
    
    // 测试字符串序列化
    std::string test_str = "Hello World";
    std::string serialized_str = SerializationUtils::serialize_string(test_str);
    std::string deserialized_str = SerializationUtils::deserialize_string(serialized_str);
    EXPECT_EQ(deserialized_str, test_str);
    
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

// 服务注册中心测试
TEST_F(RpcFrameworkSimpleTest, ServiceRegistry) {
    auto& registry = ServiceRegistry::get_instance();
    
    // 注册服务
    registry.register_service("TestService", "127.0.0.1", 8080);
    registry.register_service("TestService", "127.0.0.2", 8081);
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
TEST_F(RpcFrameworkSimpleTest, LoadBalancing) {
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

// 消息类型字符串测试
TEST_F(RpcFrameworkSimpleTest, MessageTypeString) {
    EXPECT_EQ(get_message_type_string(MessageType::REQUEST), "REQUEST");
    EXPECT_EQ(get_message_type_string(MessageType::RESPONSE), "RESPONSE");
    EXPECT_EQ(get_message_type_string(MessageType::ERROR), "ERROR");
    EXPECT_EQ(get_message_type_string(MessageType::HEARTBEAT), "HEARTBEAT");
}

// 消息ID生成测试
TEST_F(RpcFrameworkSimpleTest, MessageIdGeneration) {
    uint32_t id1 = generate_message_id();
    uint32_t id2 = generate_message_id();
    uint32_t id3 = generate_message_id();
    
    EXPECT_NE(id1, id2);
    EXPECT_NE(id2, id3);
    EXPECT_NE(id1, id3);
    
    // 验证ID是递增的
    EXPECT_LT(id1, id2);
    EXPECT_LT(id2, id3);
}

// 异常测试
TEST_F(RpcFrameworkSimpleTest, ExceptionHandling) {
    // 测试RPC异常
    try {
        throw rpc_exception("Test exception");
    } catch (const rpc_exception& e) {
        EXPECT_STREQ(e.what(), "Test exception");
    }
    
    // 测试空消息反序列化
    EXPECT_THROW(deserialize_message(""), rpc_exception);
    
    // 测试短消息反序列化
    EXPECT_THROW(deserialize_message("short"), rpc_exception);
    
    // 测试无效消息头反序列化
    std::string invalid_header(28, '\0');
    invalid_header[0] = 0x12; // 设置无效的魔数
    EXPECT_THROW(deserialize_message(invalid_header), rpc_exception);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
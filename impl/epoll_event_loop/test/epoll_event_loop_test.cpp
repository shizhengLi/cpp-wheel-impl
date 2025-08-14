#include <gtest/gtest.h>
#include <thread>
#include <chrono>
#include <atomic>
#include <vector>
#include <string>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include "epoll_event_loop.hpp"

using namespace impl;

class EpollEventLoopTest : public ::testing::Test {
protected:
    void SetUp() override {
        loop = std::make_unique<EpollEventLoop>();
    }
    
    void TearDown() override {
        if (loop && loop->is_running()) {
            loop->stop();
        }
        loop.reset();
    }
    
    std::unique_ptr<EpollEventLoop> loop;
};

// 基本功能测试
TEST_F(EpollEventLoopTest, BasicFunctionality) {
    EXPECT_FALSE(loop->is_running());
    
    // 启动事件循环
    std::thread loop_thread([this]() {
        loop->run();
    });
    
    // 等待事件循环启动
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_TRUE(loop->is_running());
    
    // 停止事件循环
    loop->stop();
    
    // 等待线程结束
    if (loop_thread.joinable()) {
        loop_thread.join();
    }
    
    EXPECT_FALSE(loop->is_running());
}

// 定时器测试
TEST_F(EpollEventLoopTest, TimerFunctionality) {
    std::atomic<int> counter{0};
    bool timer_fired = false;
    
    // 创建简单定时器
    auto timer = make_simple_timer([&counter, &timer_fired]() {
        counter++;
        timer_fired = true;
    });
    
    // 启动事件循环
    std::thread loop_thread([this]() {
        loop->run();
    });
    
    // 等待事件循环启动
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // 添加定时器（100ms后触发）
    uint64_t timer_id = loop->add_timer(100, timer);
    EXPECT_NE(timer_id, 0);
    
    // 等待定时器触发
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    
    EXPECT_TRUE(timer_fired);
    EXPECT_EQ(counter, 1);
    
    // 停止事件循环
    loop->stop();
    loop_thread.join();
}

// 多定时器测试
TEST_F(EpollEventLoopTest, MultipleTimers) {
    std::atomic<int> counter{0};
    std::vector<bool> timer_fired(3, false);
    
    // 启动事件循环
    std::thread loop_thread([this]() {
        loop->run();
    });
    
    // 等待事件循环启动
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // 添加多个定时器
    for (int i = 0; i < 3; ++i) {
        auto timer = make_simple_timer([&counter, &timer_fired, i]() {
            counter++;
            timer_fired[i] = true;
        });
        
        loop->add_timer(100 + i * 50, timer);
    }
    
    // 等待所有定时器触发
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    
    EXPECT_EQ(counter, 3);
    EXPECT_TRUE(timer_fired[0]);
    EXPECT_TRUE(timer_fired[1]);
    EXPECT_TRUE(timer_fired[2]);
    
    // 停止事件循环
    loop->stop();
    loop_thread.join();
}

// 定时器取消测试
TEST_F(EpollEventLoopTest, TimerCancellation) {
    std::atomic<int> counter{0};
    bool timer_fired = false;
    
    // 启动事件循环
    std::thread loop_thread([this]() {
        loop->run();
    });
    
    // 等待事件循环启动
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // 创建定时器
    auto timer = make_simple_timer([&counter, &timer_fired]() {
        counter++;
        timer_fired = true;
    });
    
    // 添加定时器（200ms后触发）
    uint64_t timer_id = loop->add_timer(200, timer);
    
    // 在100ms后取消定时器
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    loop->cancel_timer(timer_id);
    
    // 再等待200ms
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    
    EXPECT_FALSE(timer_fired);
    EXPECT_EQ(counter, 0);
    
    // 停止事件循环
    loop->stop();
    loop_thread.join();
}

// TCP服务器测试
TEST_F(EpollEventLoopTest, TCPServer) {
    std::atomic<int> accept_count{0};
    std::atomic<int> client_count{0};
    
    // 创建接受连接的处理器
    auto accept_handler = make_simple_handler(
        [this, &accept_count, &client_count](int server_fd) {
            accept_count++;
            
            // 接受连接
            struct sockaddr_in client_addr;
            socklen_t client_len = sizeof(client_addr);
            int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
            
            if (client_fd != -1) {
                client_count++;
                
                // 创建客户端处理器
                auto client_handler = make_simple_handler(
                    [client_fd](int fd) {
                        char buffer[1024];
                        ssize_t bytes_read = read(fd, buffer, sizeof(buffer));
                        if (bytes_read > 0) {
                            // 简单回显
                            write(fd, buffer, bytes_read);
                        } else {
                            close(fd);
                        }
                    },
                    [](int fd, const std::string& error) {
                        (void)error; // 避免未使用参数警告
                        close(fd);
                    }
                );
                
                // 添加客户端到事件循环
                loop->add_fd(client_fd, EPOLLIN, client_handler);
            }
        },
        [](int fd, const std::string& error) {
            (void)fd; (void)error; // 避免未使用参数警告
            std::cerr << "Server error: " << error << std::endl;
        }
    );
    
    // 启动事件循环
    std::thread loop_thread([this]() {
        loop->run();
    });
    
    // 等待事件循环启动
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // 创建TCP服务器
    int server_port = 12345;
    int server_fd = loop->create_tcp_server(server_port, accept_handler);
    EXPECT_NE(server_fd, -1);
    
    // 创建客户端连接
    std::vector<std::thread> client_threads;
    for (int i = 0; i < 3; ++i) {
        client_threads.emplace_back([server_port, i]() {
            // 创建客户端socket
            int client_fd = socket(AF_INET, SOCK_STREAM, 0);
            EXPECT_NE(client_fd, -1);
            
            struct sockaddr_in server_addr;
            server_addr.sin_family = AF_INET;
            server_addr.sin_port = htons(server_port);
            inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);
            
            // 连接服务器
            EXPECT_EQ(connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)), 0);
            
            // 发送数据
            std::string message = "Hello from client " + std::to_string(i);
            write(client_fd, message.c_str(), message.size());
            
            // 读取响应
            char buffer[1024];
            ssize_t bytes_read = read(client_fd, buffer, sizeof(buffer));
            EXPECT_GT(bytes_read, 0);
            
            // 验证响应
            std::string response(buffer, bytes_read);
            EXPECT_EQ(response, message);
            
            close(client_fd);
        });
    }
    
    // 等待所有客户端完成
    for (auto& thread : client_threads) {
        thread.join();
    }
    
    // 等待处理完成
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    
    EXPECT_EQ(accept_count, 3);
    EXPECT_EQ(client_count, 3);
    
    // 停止事件循环
    loop->stop();
    loop_thread.join();
    
    // 关闭服务器socket
    close(server_fd);
}

// UDP测试
TEST_F(EpollEventLoopTest, UDPFunctionality) {
    std::atomic<int> receive_count{0};
    std::vector<std::string> received_messages;
    
    // 创建UDP处理器
    auto udp_handler = make_simple_handler(
        [&receive_count, &received_messages](int udp_fd) {
            receive_count++;
            
            char buffer[1024];
            struct sockaddr_in client_addr;
            socklen_t client_len = sizeof(client_addr);
            
            ssize_t bytes_read = recvfrom(udp_fd, buffer, sizeof(buffer), 0,
                                         (struct sockaddr*)&client_addr, &client_len);
            
            if (bytes_read > 0) {
                std::string message(buffer, bytes_read);
                received_messages.push_back(message);
                
                // 发送响应
                std::string response = "Echo: " + message;
                sendto(udp_fd, response.c_str(), response.size(), 0,
                       (struct sockaddr*)&client_addr, client_len);
            }
        },
        [](int fd, const std::string& error) {
            (void)fd; (void)error; // 避免未使用参数警告
            std::cerr << "UDP error: " << error << std::endl;
        }
    );
    
    // 启动事件循环
    std::thread loop_thread([this]() {
        loop->run();
    });
    
    // 等待事件循环启动
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // 创建UDP socket
    int udp_fd = loop->create_udp_socket(udp_handler);
    EXPECT_NE(udp_fd, -1);
    
    // 绑定UDP socket
    struct sockaddr_in udp_addr;
    udp_addr.sin_family = AF_INET;
    udp_addr.sin_addr.s_addr = INADDR_ANY;
    udp_addr.sin_port = htons(12346);
    
    EXPECT_EQ(bind(udp_fd, (struct sockaddr*)&udp_addr, sizeof(udp_addr)), 0);
    
    // 创建客户端发送UDP消息
    std::vector<std::thread> client_threads;
    for (int i = 0; i < 3; ++i) {
        client_threads.emplace_back([i]() {
            // 创建UDP socket
            int client_fd = socket(AF_INET, SOCK_DGRAM, 0);
            EXPECT_NE(client_fd, -1);
            
            struct sockaddr_in server_addr;
            server_addr.sin_family = AF_INET;
            server_addr.sin_port = htons(12346);
            inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);
            
            // 发送消息
            std::string message = "UDP message " + std::to_string(i);
            sendto(client_fd, message.c_str(), message.size(), 0,
                   (struct sockaddr*)&server_addr, sizeof(server_addr));
            
            // 接收响应
            char buffer[1024];
            struct sockaddr_in from_addr;
            socklen_t from_len = sizeof(from_addr);
            
            ssize_t bytes_read = recvfrom(client_fd, buffer, sizeof(buffer), 0,
                                         (struct sockaddr*)&from_addr, &from_len);
            
            EXPECT_GT(bytes_read, 0);
            std::string response(buffer, bytes_read);
            EXPECT_EQ(response, "Echo: " + message);
            
            close(client_fd);
        });
    }
    
    // 等待所有客户端完成
    for (auto& thread : client_threads) {
        thread.join();
    }
    
    // 等待处理完成
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    
    EXPECT_EQ(receive_count, 3);
    EXPECT_EQ(received_messages.size(), 3);
    
    // 停止事件循环
    loop->stop();
    loop_thread.join();
    
    // 关闭UDP socket
    close(udp_fd);
}

// 统计信息测试
TEST_F(EpollEventLoopTest, Stats) {
    std::string stats = loop->get_stats();
    EXPECT_FALSE(stats.empty());
    EXPECT_NE(stats.find("EpollEventLoop Stats:"), std::string::npos);
    EXPECT_NE(stats.find("Running: No"), std::string::npos);
    
    std::cout << stats << std::endl;
}

// 并发测试
TEST_F(EpollEventLoopTest, Concurrency) {
    std::atomic<int> counter{0};
    const int num_timers = 10;
    
    // 启动事件循环
    std::thread loop_thread([this]() {
        loop->run();
    });
    
    // 等待事件循环启动
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // 创建多个定时器
    for (int i = 0; i < num_timers; ++i) {
        auto timer = make_simple_timer([&counter]() {
            counter++;
        });
        
        loop->add_timer(50 + i * 10, timer);
    }
    
    // 等待所有定时器触发
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    
    EXPECT_EQ(counter, num_timers);
    
    // 停止事件循环
    loop->stop();
    loop_thread.join();
}

// 错误处理测试
TEST_F(EpollEventLoopTest, ErrorHandling) {
    // 测试无效文件描述符
    EXPECT_THROW(loop->add_fd(-1, EPOLLIN, nullptr), epoll_event_loop_exception);
    
    // 测试重复添加
    int pipe_fds[2];
    EXPECT_EQ(pipe(pipe_fds), 0);
    
    auto handler = make_simple_handler(
        [](int fd) {
            char buffer[1];
            read(fd, buffer, sizeof(buffer));
        },
        [](int fd, const std::string& error) {
            (void)error; // 避免未使用参数警告
            close(fd);
        }
    );
    
    // 第一次添加应该成功
    EXPECT_NO_THROW(loop->add_fd(pipe_fds[0], EPOLLIN, handler));
    
    // 第二次添加应该失败
    EXPECT_THROW(loop->add_fd(pipe_fds[0], EPOLLIN, handler), epoll_event_loop_exception);
    
    // 清理
    close(pipe_fds[0]);
    close(pipe_fds[1]);
}

// 长时间运行测试
TEST_F(EpollEventLoopTest, LongRunning) {
    std::atomic<int> counter{0};
    std::atomic<bool> stop_flag{false};
    
    // 启动事件循环
    std::thread loop_thread([this]() {
        loop->run();
    });
    
    // 等待事件循环启动
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // 创建定期触发的定时器
    auto periodic_timer = make_simple_timer([&counter, &stop_flag]() {
        if (!stop_flag) {
            counter++;
        }
    });
    
    // 添加多个定时器
    for (int i = 0; i < 5; ++i) {
        loop->add_timer(100, periodic_timer);
    }
    
    // 让定时器运行一段时间
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    
    // 停止定时器
    stop_flag = true;
    
    // 等待最后的定时器触发
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    
    EXPECT_GT(counter, 0);
    
    // 停止事件循环
    loop->stop();
    loop_thread.join();
}

// 边缘触发测试
TEST_F(EpollEventLoopTest, EdgeTriggered) {
    std::atomic<int> read_count{0};
    
    // 创建管道
    int pipe_fds[2];
    EXPECT_EQ(pipe(pipe_fds), 0);
    
    // 设置非阻塞
    EpollEventLoop::set_nonblocking(pipe_fds[0]);
    EpollEventLoop::set_nonblocking(pipe_fds[1]);
    
    // 创建边缘触发的处理器
    auto et_handler = make_simple_handler(
        [&read_count, pipe_fds](int fd) {
            read_count++;
            
            // 读取所有可用数据
            char buffer[1024];
            ssize_t bytes_read;
            do {
                bytes_read = read(fd, buffer, sizeof(buffer));
            } while (bytes_read > 0);
        },
        [](int fd, const std::string& error) {
            (void)error; // 避免未使用参数警告
            close(fd);
        }
    );
    
    // 启动事件循环
    std::thread loop_thread([this]() {
        loop->run();
    });
    
    // 等待事件循环启动
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // 添加边缘触发的文件描述符
    loop->add_fd(pipe_fds[0], EPOLLIN, et_handler, true);
    
    // 写入数据
    write(pipe_fds[1], "Hello", 5);
    
    // 等待事件处理
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // 再次写入数据
    write(pipe_fds[1], "World", 5);
    
    // 等待事件处理
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    EXPECT_EQ(read_count, 2);
    
    // 停止事件循环
    loop->stop();
    loop_thread.join();
    
    // 清理
    close(pipe_fds[0]);
    close(pipe_fds[1]);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
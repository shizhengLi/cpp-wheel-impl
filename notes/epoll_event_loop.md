# Epoll Event Loop 实现文档

## 项目概述

本项目实现了一个基于epoll的高效事件循环系统，用于处理大量的并发IO操作和定时器任务。该组件是构建高性能网络服务器的基础设施。

## 设计思路

### 核心概念

1. **IO多路复用**：使用epoll系统调用实现高效的IO事件监控
2. **事件驱动架构**：基于事件回调的非阻塞IO处理
3. **定时器管理**：内置定时器堆，支持定时任务调度
4. **线程安全**：支持多线程环境下的安全操作
5. **网络编程支持**：提供TCP/UDP服务器和客户端创建接口

### 架构设计

```
EpollEventLoop
├── 事件管理
│   ├── epoll_fd: epoll实例文件描述符
│   ├── events: 事件数组
│   └── fd_map: 文件描述符映射
├── 定时器管理
│   ├── timers: 定时器堆
│   ├── timer_thread: 定时器线程
│   └── timer_cv: 定时器条件变量
├── 同步机制
│   ├── fd_mutex: 文件描述符互斥锁
│   └── timer_mutex: 定时器互斥锁
└── 状态控制
    ├── running_: 运行标志
    └── stopped_: 停止标志
```

## 技术要点

### 1. epoll基础机制

#### epoll系统调用
- `epoll_create1()`: 创建epoll实例
- `epoll_ctl()`: 添加/修改/删除文件描述符
- `epoll_wait()`: 等待事件发生
- 支持边缘触发(ET)和水平触发(LT)模式

#### 事件处理机制
```cpp
struct epoll_event {
    uint32_t events;  // 事件类型
    epoll_data_t data; // 用户数据
};
```

### 2. 定时器管理

#### 定时器堆结构
- 使用小顶堆管理定时器
- 按照到期时间排序
- 支持动态添加和取消定时器

#### 定时器线程
- 独立线程处理定时器超时
- 使用条件变量实现精确等待
- 支持高精度定时器

### 3. 网络编程支持

#### TCP服务器
- 自动创建监听socket
- 非阻塞IO设置
- 连接接受和数据处理

#### TCP客户端
- 异步连接建立
- 连接状态监控
- 数据读写处理

#### UDP支持
- 无连接数据报处理
- 支持多播和广播
- 高效的数据包处理

### 4. 异常处理

#### 错误处理策略
- 文件描述符错误自动关闭
- 定时器异常不影响其他定时器
- 网络错误自动恢复

#### 资源管理
- RAII设计模式
- 自动清理资源
- 内存泄漏检测

## 接口说明

### 核心接口

```cpp
class EpollEventLoop {
public:
    // 构造和析构
    explicit EpollEventLoop(int max_events = 1024, int timeout = 100);
    ~EpollEventLoop();
    
    // 文件描述符管理
    void add_fd(int fd, uint32_t events, std::shared_ptr<EventHandler> handler, bool is_et = false);
    void modify_fd(int fd, uint32_t events);
    void remove_fd(int fd);
    
    // 定时器管理
    uint64_t add_timer(uint64_t delay, std::shared_ptr<Timer> timer);
    void cancel_timer(uint64_t timer_id);
    
    // 事件循环控制
    void run();
    void stop();
    bool is_running() const;
    
    // 查询接口
    std::string get_stats() const;
    
    // 网络编程
    int create_tcp_server(int port, std::shared_ptr<EventHandler> accept_handler);
    int create_tcp_client(const std::string& ip, int port, std::shared_ptr<EventHandler> connect_handler);
    int create_udp_socket(std::shared_ptr<EventHandler> handler);
    
    // 工具函数
    static void set_nonblocking(int fd);
    static void set_reuseaddr(int fd);
};
```

### 事件处理器接口

```cpp
class EventHandler {
public:
    virtual ~EventHandler() = default;
    virtual void handle_event(int fd, uint32_t events) = 0;
    virtual void handle_error(int fd, const std::string& error) = 0;
};
```

### 定时器接口

```cpp
class Timer {
public:
    virtual ~Timer() = default;
    virtual void on_timeout() = 0;
    virtual bool is_canceled() const = 0;
    virtual void cancel() = 0;
};
```

## 使用示例

### 基本事件循环使用

```cpp
#include "epoll_event_loop.hpp"
#include <iostream>

int main() {
    // 创建事件循环
    impl::EpollEventLoop loop;
    
    // 启动事件循环线程
    std::thread loop_thread([&loop]() {
        loop.run();
    });
    
    // 创建简单的事件处理器
    auto handler = impl::make_simple_handler(
        [](int fd) {
            char buffer[1024];
            ssize_t bytes_read = read(fd, buffer, sizeof(buffer));
            if (bytes_read > 0) {
                std::cout << "Received: " << std::string(buffer, bytes_read) << std::endl;
            }
        },
        [](int fd, const std::string& error) {
            std::cerr << "Error: " << error << std::endl;
            close(fd);
        }
    );
    
    // 添加文件描述符到事件循环
    int fd = /* 获取文件描述符 */;
    loop.add_fd(fd, EPOLLIN, handler);
    
    // 运行一段时间后停止
    std::this_thread::sleep_for(std::chrono::seconds(10));
    loop.stop();
    loop_thread.join();
    
    return 0;
}
```

### TCP服务器使用

```cpp
#include "epoll_event_loop.hpp"
#include <iostream>

int main() {
    impl::EpollEventLoop loop;
    
    // 启动事件循环
    std::thread loop_thread([&loop]() {
        loop.run();
    });
    
    // 创建接受连接的处理器
    auto accept_handler = impl::make_simple_handler(
        [&loop](int server_fd) {
            // 接受新连接
            struct sockaddr_in client_addr;
            socklen_t client_len = sizeof(client_addr);
            int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
            
            if (client_fd != -1) {
                std::cout << "New connection from " 
                          << inet_ntoa(client_addr.sin_addr) << std::endl;
                
                // 创建客户端处理器
                auto client_handler = impl::make_simple_handler(
                    [](int fd) {
                        char buffer[1024];
                        ssize_t bytes_read = read(fd, buffer, sizeof(buffer));
                        if (bytes_read > 0) {
                            // 回显数据
                            write(fd, buffer, bytes_read);
                        } else {
                            close(fd);
                        }
                    },
                    [](int fd, const std::string& error) {
                        std::cerr << "Client error: " << error << std::endl;
                        close(fd);
                    }
                );
                
                // 添加客户端到事件循环
                loop.add_fd(client_fd, EPOLLIN, client_handler);
            }
        },
        [](int fd, const std::string& error) {
            std::cerr << "Server error: " << error << std::endl;
        }
    );
    
    // 创建TCP服务器
    int server_fd = loop.create_tcp_server(8080, accept_handler);
    std::cout << "Server started on port 8080" << std::endl;
    
    // 运行服务器
    std::this_thread::sleep_for(std::chrono::minutes(30));
    
    // 停止服务器
    loop.stop();
    loop_thread.join();
    close(server_fd);
    
    return 0;
}
```

### 定时器使用

```cpp
#include "epoll_event_loop.hpp"
#include <iostream>

int main() {
    impl::EpollEventLoop loop;
    
    // 启动事件循环
    std::thread loop_thread([&loop]() {
        loop.run();
    });
    
    // 创建定时器
    auto timer = impl::make_simple_timer([]() {
        std::cout << "Timer fired at " 
                  << std::chrono::system_clock::now().time_since_epoch().count() 
                  << std::endl;
    });
    
    // 添加定时器（1秒后触发）
    uint64_t timer_id = loop.add_timer(1000, timer);
    
    // 创建周期性定时器
    auto periodic_timer = impl::make_simple_timer([&loop]() {
        static int count = 0;
        std::cout << "Periodic timer " << count++ << std::endl;
        
        if (count >= 5) {
            loop.stop();
        }
    });
    
    // 添加多个周期性定时器
    for (int i = 0; i < 5; ++i) {
        loop.add_timer(1000 + i * 200, periodic_timer);
    }
    
    // 等待事件循环结束
    loop_thread.join();
    
    return 0;
}
```

### UDP服务器使用

```cpp
#include "epoll_event_loop.hpp"
#include <iostream>

int main() {
    impl::EpollEventLoop loop;
    
    // 启动事件循环
    std::thread loop_thread([&loop]() {
        loop.run();
    });
    
    // 创建UDP处理器
    auto udp_handler = impl::make_simple_handler(
        [](int udp_fd) {
            char buffer[1024];
            struct sockaddr_in client_addr;
            socklen_t client_len = sizeof(client_addr);
            
            ssize_t bytes_read = recvfrom(udp_fd, buffer, sizeof(buffer), 0,
                                       (struct sockaddr*)&client_addr, &client_len);
            
            if (bytes_read > 0) {
                std::string message(buffer, bytes_read);
                std::cout << "UDP message from " 
                          << inet_ntoa(client_addr.sin_addr) 
                          << ": " << message << std::endl;
                
                // 发送响应
                std::string response = "Echo: " + message;
                sendto(udp_fd, response.c_str(), response.size(), 0,
                       (struct sockaddr*)&client_addr, client_len);
            }
        },
        [](int fd, const std::string& error) {
            std::cerr << "UDP error: " << error << std::endl;
        }
    );
    
    // 创建UDP socket
    int udp_fd = loop.create_udp_socket(udp_handler);
    
    // 绑定UDP socket
    struct sockaddr_in udp_addr;
    udp_addr.sin_family = AF_INET;
    udp_addr.sin_addr.s_addr = INADDR_ANY;
    udp_addr.sin_port = htons(9090);
    
    if (bind(udp_fd, (struct sockaddr*)&udp_addr, sizeof(udp_addr)) == 0) {
        std::cout << "UDP server started on port 9090" << std::endl;
        
        // 运行服务器
        std::this_thread::sleep_for(std::chrono::minutes(10));
    }
    
    // 停止服务器
    loop.stop();
    loop_thread.join();
    close(udp_fd);
    
    return 0;
}
```

## 性能分析

### 时间复杂度

| 操作 | 时间复杂度 | 说明 |
|------|------------|------|
| `add_fd()` | O(1) | 添加文件描述符到epoll |
| `modify_fd()` | O(1) | 修改文件描述符事件 |
| `remove_fd()` | O(1) | 从epoll中移除文件描述符 |
| `add_timer()` | O(log n) | 添加定时器到堆中 |
| `cancel_timer()` | O(n) | 查找并取消定时器 |
| `epoll_wait()` | O(n) | n为活跃事件数 |

### 空间复杂度

- 每个文件描述符需要存储处理器信息和事件状态
- 定时器堆的空间复杂度为O(n)，n为定时器数量
- 事件数组的大小由max_events参数决定

### 性能优化策略

1. **边缘触发模式**：减少不必要的系统调用
2. **非阻塞IO**：避免阻塞事件循环
3. **批量处理**：一次处理多个事件
4. **定时器优化**：使用堆结构提高效率
5. **内存预分配**：避免频繁的内存分配

## 测试策略

### 单元测试覆盖

1. **基本功能测试**：验证事件循环的启动和停止
2. **定时器测试**：验证定时器的添加、触发和取消
3. **网络测试**：验证TCP/UDP服务器和客户端功能
4. **并发测试**：验证多线程环境下的安全性
5. **错误处理测试**：验证异常情况的处理
6. **性能测试**：验证系统在高负载下的表现

### 测试结果

```
[==========] Running 11 tests from 1 test suite.
[----------] 11 tests from EpollEventLoopTest
[ RUN      ] EpollEventLoopTest.BasicFunctionality
[       OK ] EpollEventLoopTest.BasicFunctionality (100 ms)
[ RUN      ] EpollEventLoopTest.TimerFunctionality
[       OK ] EpollEventLoopTest.TimerFunctionality (300 ms)
[ RUN      ] EpollEventLoopTest.MultipleTimers
[       OK ] EpollEventLoopTest.MultipleTimers (400 ms)
[ RUN      ] EpollEventLoopTest.TimerCancellation
[       OK ] EpollEventLoopTest.TimerCancellation (400 ms)
[ RUN      ] EpollEventLoopTest.TCPServer
[       OK ] EpollEventLoopTest.TCPServer (301 ms)
[ RUN      ] EpollEventLoopTest.UDPFunctionality
[       OK ] EpollEventLoopTest.UDPFunctionality (301 ms)
[ RUN      ] EpollEventLoopTest.Stats
[       OK ] EpollEventLoopTest.Stats (0 ms)
[ RUN      ] EpollEventLoopTest.Concurrency
[       OK ] EpollEventLoopTest.Concurrency (400 ms)
[ RUN      ] EpollEventLoopTest.ErrorHandling
[       OK ] EpollEventLoopTest.ErrorHandling (0 ms)
[ RUN      ] EpollEventLoopTest.LongRunning
[       OK ] EpollEventLoopTest.LongRunning (801 ms)
[ RUN      ] EpollEventLoopTest.EdgeTriggered
[       OK ] EpollEventLoopTest.EdgeTriggered (300 ms)
[----------] 11 tests from EpollEventLoopTest (3308 ms total)

[  PASSED  ] 11 tests.
```

关键测试项：
- ✅ 基本事件循环功能
- ✅ 定时器管理和调度
- ✅ TCP服务器功能
- ✅ UDP通信功能
- ✅ 并发安全性
- ✅ 错误处理机制
- ✅ 长时间运行稳定性

## 常见问题和解决方案

### 1. 文件描述符泄漏

**问题**：忘记关闭文件描述符导致资源泄漏

**解决方案**：
- 在错误处理器中自动关闭文件描述符
- 使用RAII包装文件描述符
- 定期检查文件描述符使用情况

### 2. 定时器精度问题

**问题**：定时器触发时间不准确

**解决方案**：
- 使用高精度时钟
- 优化定时器线程调度
- 避免在定时器回调中执行耗时操作

### 3. 内存泄漏

**问题**：事件处理器或定时器对象泄漏

**解决方案**：
- 使用智能指针管理对象生命周期
- 在移除文件描述符时清理相关资源
- 定期检查内存使用情况

### 4. 线程安全问题

**问题**：多线程环境下的竞争条件

**解决方案**：
- 使用互斥锁保护共享数据
- 最小化临界区范围
- 使用原子操作处理简单状态

### 5. 性能瓶颈

**问题**：事件循环处理速度跟不上事件产生速度

**解决方案**：
- 使用边缘触发模式
- 优化事件处理逻辑
- 考虑多线程事件循环

## 优化建议

### 1. 进一步优化方向

1. **多线程事件循环**：支持多个事件循环线程
2. **连接池**：复用TCP连接提高性能
3. **SSL/TLS支持**：添加安全通信功能
4. **HTTP协议支持**：构建HTTP服务器框架
5. **负载均衡**：支持多进程/多线程负载均衡

### 2. 使用建议

1. **适用场景**：
   - 高并发网络服务器
   - 实时通信系统
   - 游戏服务器
   - 代理服务器

2. **不适用场景**：
   - 单线程简单应用
   - CPU密集型任务
   - 需要复杂同步逻辑的场景

3. **最佳实践**：
   - 合理设置epoll_wait超时时间
   - 使用非阻塞IO
   - 避免在事件回调中执行耗时操作
   - 及时处理错误和异常

## 总结

本epoll事件循环实现具有以下特点：

- **高性能**：基于epoll的高效IO多路复用
- **功能完整**：支持定时器、TCP、UDP等完整功能
- **线程安全**：支持多线程环境下的安全操作
- **易用性**：提供简洁的接口和丰富的工具函数
- **可靠性**：完善的异常处理和资源管理
- **可扩展性**：支持自定义事件处理器和定时器

通过精心的架构设计和实现细节，这个事件循环能够在各种高并发场景下提供稳定高效的性能表现，是构建高性能网络服务的优秀基础设施。

## 实现位置

- **头文件**：`impl/epoll_event_loop/include/epoll_event_loop.hpp`
- **实现文件**：`impl/epoll_event_loop/include/epoll_event_loop.cpp`
- **测试文件**：`impl/epoll_event_loop/test/epoll_event_loop_test.cpp`
- **构建配置**：`impl/epoll_event_loop/CMakeLists.txt`
- **文档**：`notes/epoll_event_loop.md`
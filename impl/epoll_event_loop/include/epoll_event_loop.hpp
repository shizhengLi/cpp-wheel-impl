#pragma once

#include <sys/epoll.h>
#include <functional>
#include <unordered_map>
#include <vector>
#include <memory>
#include <chrono>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <stdexcept>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

namespace impl {

/**
 * @brief epoll事件循环异常类
 */
class epoll_event_loop_exception : public std::runtime_error {
public:
    explicit epoll_event_loop_exception(const std::string& msg) : std::runtime_error(msg) {}
};

/**
 * @brief 事件类型枚举
 */
enum class EventType {
    READ = EPOLLIN,
    WRITE = EPOLLOUT,
    ERROR = EPOLLERR,
    HANGUP = EPOLLHUP,
    ONESHOT = EPOLLONESHOT
};

/**
 * @brief 事件处理器接口
 */
class EventHandler {
public:
    virtual ~EventHandler() = default;
    virtual void handle_event(int fd, uint32_t events) = 0;
    virtual void handle_error(int fd, const std::string& error) = 0;
};

/**
 * @brief 定时器接口
 */
class Timer {
public:
    virtual ~Timer() = default;
    virtual void on_timeout() = 0;
    virtual bool is_canceled() const = 0;
    virtual void cancel() = 0;
};

/**
 * @brief epoll事件循环实现
 * 
 * 特点：
 * - 基于epoll的高效IO多路复用
 * - 支持边缘触发和水平触发模式
 * - 内置定时器管理
 * - 线程安全的事件处理
 * - 支持TCP/UDP网络编程
 * - 异步IO处理能力
 */
class EpollEventLoop {
public:
    /**
     * @brief 构造函数
     * @param max_events 最大事件数
     * @param timeout 超时时间（毫秒）
     */
    explicit EpollEventLoop(int max_events = 1024, int timeout = 100);
    
    /**
     * @brief 析构函数
     */
    ~EpollEventLoop();
    
    /**
     * @brief 添加文件描述符到epoll
     * @param fd 文件描述符
     * @param events 事件类型
     * @param handler 事件处理器
     * @param is_et 是否使用边缘触发
     */
    void add_fd(int fd, uint32_t events, std::shared_ptr<EventHandler> handler, bool is_et = false);
    
    /**
     * @brief 修改文件描述符的事件
     * @param fd 文件描述符
     * @param events 新的事件类型
     */
    void modify_fd(int fd, uint32_t events);
    
    /**
     * @brief 从epoll中移除文件描述符
     * @param fd 文件描述符
     */
    void remove_fd(int fd);
    
    /**
     * @brief 添加定时器
     * @param delay 延迟时间（毫秒）
     * @param timer 定时器对象
     * @return 定时器ID
     */
    uint64_t add_timer(uint64_t delay, std::shared_ptr<Timer> timer);
    
    /**
     * @brief 取消定时器
     * @param timer_id 定时器ID
     */
    void cancel_timer(uint64_t timer_id);
    
    /**
     * @brief 启动事件循环
     */
    void run();
    
    /**
     * @brief 停止事件循环
     */
    void stop();
    
    /**
     * @brief 检查事件循环是否正在运行
     * @return 是否正在运行
     */
    bool is_running() const;
    
    /**
     * @brief 获取统计信息
     * @return 统计信息字符串
     */
    std::string get_stats() const;
    
    /**
     * @brief 创建TCP服务器
     * @param port 端口号
     * @param accept_handler 接受连接处理器
     * @return 监听socket文件描述符
     */
    int create_tcp_server(int port, std::shared_ptr<EventHandler> accept_handler);
    
    /**
     * @brief 创建TCP客户端
     * @param ip 目标IP地址
     * @param port 目标端口
     * @param connect_handler 连接处理器
     * @return 连接socket文件描述符
     */
    int create_tcp_client(const std::string& ip, int port, std::shared_ptr<EventHandler> connect_handler);
    
    /**
     * @brief 创建UDP socket
     * @param handler 事件处理器
     * @return UDP socket文件描述符
     */
    int create_udp_socket(std::shared_ptr<EventHandler> handler);
    
    /**
     * @brief 设置非阻塞模式
     * @param fd 文件描述符
     */
    static void set_nonblocking(int fd);
    
    /**
     * @brief 设置socket重用地址
     * @param fd 文件描述符
     */
    static void set_reuseaddr(int fd);
    
    // 禁用拷贝构造和拷贝赋值
    EpollEventLoop(const EpollEventLoop&) = delete;
    EpollEventLoop& operator=(const EpollEventLoop&) = delete;

private:
    /**
     * @brief 定时器项结构
     */
    struct TimerItem {
        uint64_t id;
        uint64_t expire_time;
        std::shared_ptr<Timer> timer;
        bool canceled;
        
        bool operator<(const TimerItem& other) const {
            return expire_time > other.expire_time; // 小顶堆
        }
    };
    
    /**
     * @brief 文件描述符信息结构
     */
    struct FdInfo {
        std::shared_ptr<EventHandler> handler;
        uint32_t events;
        bool is_et;
    };
    
    /**
     * @brief 处理epoll事件
     */
    void handle_events();
    
    /**
     * @brief 处理定时器
     */
    void handle_timers();
    
    /**
     * @brief 检查定时器超时
     */
    void check_timer_timeout();
    
    /**
     * @brief 生成定时器ID
     * @return 新的定时器ID
     */
    uint64_t generate_timer_id();
    
    /**
     * @brief 获取当前时间戳（毫秒）
     * @return 当前时间戳
     */
    static uint64_t get_current_time_ms();
    
    int epoll_fd_;                           // epoll文件描述符
    int max_events_;                         // 最大事件数
    int timeout_;                            // 超时时间
    std::unique_ptr<epoll_event[]> events_;  // 事件数组
    
    std::unordered_map<int, FdInfo> fd_map_; // 文件描述符映射
    std::mutex fd_mutex_;                     // 文件描述符映射互斥锁
    
    std::vector<TimerItem> timers_;          // 定时器堆
    std::mutex timer_mutex_;                 // 定时器互斥锁
    std::condition_variable timer_cv_;       // 定时器条件变量
    
    std::atomic<bool> running_;              // 运行标志
    std::atomic<bool> stopped_;              // 停止标志
    std::atomic<uint64_t> next_timer_id_;    // 下一个定时器ID
    
    std::atomic<uint64_t> total_events_;     // 总事件数
    std::atomic<uint64_t> total_timers_;     // 总定时器数
    
    std::thread timer_thread_;               // 定时器线程
};

/**
 * @brief 简单的事件处理器适配器
 */
template <typename ReadFunc, typename ErrorFunc>
class SimpleEventHandler : public EventHandler {
public:
    SimpleEventHandler(ReadFunc read_func, ErrorFunc error_func)
        : read_func_(read_func), error_func_(error_func) {}
    
    void handle_event(int fd, uint32_t events) override {
        if (events & EPOLLIN) {
            read_func_(fd);
        }
    }
    
    void handle_error(int fd, const std::string& error) override {
        error_func_(fd, error);
    }
    
private:
    ReadFunc read_func_;
    ErrorFunc error_func_;
};

/**
 * @brief 简单的定时器适配器
 */
template <typename TimeoutFunc>
class SimpleTimer : public Timer {
public:
    SimpleTimer(TimeoutFunc timeout_func) : timeout_func_(timeout_func), canceled_(false) {}
    
    void on_timeout() override {
        if (!canceled_) {
            timeout_func_();
        }
    }
    
    bool is_canceled() const override {
        return canceled_;
    }
    
    void cancel() override {
        canceled_ = true;
    }
    
private:
    TimeoutFunc timeout_func_;
    std::atomic<bool> canceled_;
};

/**
 * @brief 创建简单事件处理器的工厂函数
 */
template <typename ReadFunc, typename ErrorFunc>
std::shared_ptr<EventHandler> make_simple_handler(ReadFunc read_func, ErrorFunc error_func) {
    return std::make_shared<SimpleEventHandler<ReadFunc, ErrorFunc>>(read_func, error_func);
}

/**
 * @brief 创建简单定时器的工厂函数
 */
template <typename TimeoutFunc>
std::shared_ptr<Timer> make_simple_timer(TimeoutFunc timeout_func) {
    return std::make_shared<SimpleTimer<TimeoutFunc>>(timeout_func);
}

} // namespace impl
#include "epoll_event_loop.hpp"
#include <algorithm>
#include <iostream>
#include <sstream>

namespace impl {

EpollEventLoop::EpollEventLoop(int max_events, int timeout)
    : max_events_(max_events)
    , timeout_(timeout)
    , running_(false)
    , stopped_(false)
    , next_timer_id_(1)
    , total_events_(0)
    , total_timers_(0) {
    
    // 创建epoll实例
    epoll_fd_ = epoll_create1(0);
    if (epoll_fd_ == -1) {
        throw epoll_event_loop_exception("Failed to create epoll instance: " + std::string(strerror(errno)));
    }
    
    // 分配事件数组
    events_ = std::make_unique<epoll_event[]>(max_events_);
    
    // 启动定时器线程
    timer_thread_ = std::thread(&EpollEventLoop::handle_timers, this);
}

EpollEventLoop::~EpollEventLoop() {
    stop();
    
    // 等待定时器线程结束
    if (timer_thread_.joinable()) {
        timer_thread_.join();
    }
    
    // 关闭epoll文件描述符
    if (epoll_fd_ != -1) {
        close(epoll_fd_);
    }
}

void EpollEventLoop::add_fd(int fd, uint32_t events, std::shared_ptr<EventHandler> handler, bool is_et) {
    std::lock_guard<std::mutex> lock(fd_mutex_);
    
    // 设置非阻塞模式
    set_nonblocking(fd);
    
    // 创建epoll事件
    struct epoll_event ev;
    ev.events = events;
    if (is_et) {
        ev.events |= EPOLLET;
    }
    ev.data.fd = fd;
    
    // 添加到epoll
    if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd, &ev) == -1) {
        throw epoll_event_loop_exception("Failed to add fd to epoll: " + std::string(strerror(errno)));
    }
    
    // 保存文件描述符信息
    fd_map_[fd] = {handler, events, is_et};
}

void EpollEventLoop::modify_fd(int fd, uint32_t events) {
    std::lock_guard<std::mutex> lock(fd_mutex_);
    
    auto it = fd_map_.find(fd);
    if (it == fd_map_.end()) {
        throw epoll_event_loop_exception("File descriptor not found in epoll");
    }
    
    // 创建epoll事件
    struct epoll_event ev;
    ev.events = events;
    if (it->second.is_et) {
        ev.events |= EPOLLET;
    }
    ev.data.fd = fd;
    
    // 修改epoll事件
    if (epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, fd, &ev) == -1) {
        throw epoll_event_loop_exception("Failed to modify fd in epoll: " + std::string(strerror(errno)));
    }
    
    // 更新文件描述符信息
    it->second.events = events;
}

void EpollEventLoop::remove_fd(int fd) {
    std::lock_guard<std::mutex> lock(fd_mutex_);
    
    // 从epoll中移除
    if (epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, fd, nullptr) == -1) {
        throw epoll_event_loop_exception("Failed to remove fd from epoll: " + std::string(strerror(errno)));
    }
    
    // 从映射中移除
    fd_map_.erase(fd);
}

uint64_t EpollEventLoop::add_timer(uint64_t delay, std::shared_ptr<Timer> timer) {
    std::lock_guard<std::mutex> lock(timer_mutex_);
    
    uint64_t timer_id = generate_timer_id();
    uint64_t expire_time = get_current_time_ms() + delay;
    
    // 添加定时器到堆中
    timers_.push_back({timer_id, expire_time, timer, false});
    std::push_heap(timers_.begin(), timers_.end());
    
    // 通知定时器线程
    timer_cv_.notify_one();
    
    total_timers_++;
    
    return timer_id;
}

void EpollEventLoop::cancel_timer(uint64_t timer_id) {
    std::lock_guard<std::mutex> lock(timer_mutex_);
    
    // 查找并标记定时器为已取消
    for (auto& timer : timers_) {
        if (timer.id == timer_id) {
            timer.canceled = true;
            break;
        }
    }
}

void EpollEventLoop::run() {
    if (running_.exchange(true)) {
        return; // 已经在运行
    }
    
    stopped_ = false;
    
    std::cout << "Epoll event loop started..." << std::endl;
    
    while (!stopped_) {
        handle_events();
    }
    
    running_ = false;
    std::cout << "Epoll event loop stopped." << std::endl;
}

void EpollEventLoop::stop() {
    stopped_ = true;
    
    // 通知定时器线程
    timer_cv_.notify_one();
}

bool EpollEventLoop::is_running() const {
    return running_;
}

std::string EpollEventLoop::get_stats() const {
    std::lock_guard<std::mutex> fd_lock(const_cast<std::mutex&>(fd_mutex_));
    std::lock_guard<std::mutex> timer_lock(const_cast<std::mutex&>(timer_mutex_));
    
    std::stringstream ss;
    ss << "EpollEventLoop Stats:\n"
       << "  Running: " << (running_ ? "Yes" : "No") << "\n"
       << "  Epoll FD: " << epoll_fd_ << "\n"
       << "  Max Events: " << max_events_ << "\n"
       << "  Timeout: " << timeout_ << "ms\n"
       << "  Active FDs: " << fd_map_.size() << "\n"
       << "  Active Timers: " << timers_.size() << "\n"
       << "  Total Events: " << total_events_.load() << "\n"
       << "  Total Timers: " << total_timers_.load();
    
    return ss.str();
}

int EpollEventLoop::create_tcp_server(int port, std::shared_ptr<EventHandler> accept_handler) {
    // 创建socket
    int server_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (server_fd == -1) {
        throw epoll_event_loop_exception("Failed to create server socket: " + std::string(strerror(errno)));
    }
    
    // 设置重用地址
    set_reuseaddr(server_fd);
    
    // 绑定地址
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);
    
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        close(server_fd);
        throw epoll_event_loop_exception("Failed to bind server socket: " + std::string(strerror(errno)));
    }
    
    // 开始监听
    if (listen(server_fd, 128) == -1) {
        close(server_fd);
        throw epoll_event_loop_exception("Failed to listen on server socket: " + std::string(strerror(errno)));
    }
    
    // 添加到epoll
    add_fd(server_fd, EPOLLIN, accept_handler);
    
    std::cout << "TCP server started on port " << port << std::endl;
    return server_fd;
}

int EpollEventLoop::create_tcp_client(const std::string& ip, int port, std::shared_ptr<EventHandler> connect_handler) {
    // 创建socket
    int client_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (client_fd == -1) {
        throw epoll_event_loop_exception("Failed to create client socket: " + std::string(strerror(errno)));
    }
    
    // 连接服务器
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    
    if (inet_pton(AF_INET, ip.c_str(), &server_addr.sin_addr) <= 0) {
        close(client_fd);
        throw epoll_event_loop_exception("Invalid IP address: " + ip);
    }
    
    if (connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        if (errno != EINPROGRESS) {
            close(client_fd);
            throw epoll_event_loop_exception("Failed to connect to server: " + std::string(strerror(errno)));
        }
    }
    
    // 添加到epoll，监听写事件（连接完成）
    add_fd(client_fd, EPOLLOUT | EPOLLIN, connect_handler);
    
    return client_fd;
}

int EpollEventLoop::create_udp_socket(std::shared_ptr<EventHandler> handler) {
    // 创建UDP socket
    int udp_fd = socket(AF_INET, SOCK_DGRAM | SOCK_NONBLOCK, 0);
    if (udp_fd == -1) {
        throw epoll_event_loop_exception("Failed to create UDP socket: " + std::string(strerror(errno)));
    }
    
    // 添加到epoll
    add_fd(udp_fd, EPOLLIN, handler);
    
    return udp_fd;
}

void EpollEventLoop::set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        throw epoll_event_loop_exception("Failed to get file descriptor flags: " + std::string(strerror(errno)));
    }
    
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        throw epoll_event_loop_exception("Failed to set non-blocking mode: " + std::string(strerror(errno)));
    }
}

void EpollEventLoop::set_reuseaddr(int fd) {
    int opt = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        throw epoll_event_loop_exception("Failed to set SO_REUSEADDR: " + std::string(strerror(errno)));
    }
}

void EpollEventLoop::handle_events() {
    int nfds = epoll_wait(epoll_fd_, events_.get(), max_events_, timeout_);
    
    if (nfds == -1) {
        if (errno == EINTR) {
            return; // 被信号中断
        }
        throw epoll_event_loop_exception("epoll_wait failed: " + std::string(strerror(errno)));
    }
    
    if (nfds == 0) {
        return; // 超时
    }
    
    total_events_ += nfds;
    
    // 处理事件
    for (int i = 0; i < nfds; ++i) {
        int fd = events_[i].data.fd;
        uint32_t events = events_[i].events;
        
        std::shared_ptr<EventHandler> handler;
        {
            std::lock_guard<std::mutex> lock(fd_mutex_);
            auto it = fd_map_.find(fd);
            if (it != fd_map_.end()) {
                handler = it->second.handler;
            }
        }
        
        if (handler) {
            try {
                if (events & (EPOLLERR | EPOLLHUP)) {
                    handler->handle_error(fd, "Socket error or hangup");
                } else {
                    handler->handle_event(fd, events);
                }
            } catch (const std::exception& e) {
                std::cerr << "Error handling event for fd " << fd << ": " << e.what() << std::endl;
            }
        }
    }
}

void EpollEventLoop::handle_timers() {
    while (!stopped_) {
        std::unique_lock<std::mutex> lock(timer_mutex_);
        
        if (timers_.empty()) {
            timer_cv_.wait(lock, [this] { return stopped_ || !timers_.empty(); });
            continue;
        }
        
        // 计算等待时间
        uint64_t current_time = get_current_time_ms();
        uint64_t next_expire = timers_.front().expire_time;
        
        if (current_time >= next_expire) {
            check_timer_timeout();
        } else {
            timer_cv_.wait_for(lock, std::chrono::milliseconds(next_expire - current_time));
        }
    }
}

void EpollEventLoop::check_timer_timeout() {
    uint64_t current_time = get_current_time_ms();
    
    while (!timers_.empty() && timers_.front().expire_time <= current_time) {
        TimerItem timer = timers_.front();
        std::pop_heap(timers_.begin(), timers_.end());
        timers_.pop_back();
        
        if (!timer.canceled) {
            try {
                timer.timer->on_timeout();
            } catch (const std::exception& e) {
                std::cerr << "Error in timer callback: " << e.what() << std::endl;
            }
        }
    }
}

uint64_t EpollEventLoop::generate_timer_id() {
    return next_timer_id_++;
}

uint64_t EpollEventLoop::get_current_time_ms() {
    auto now = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
}

} // namespace impl
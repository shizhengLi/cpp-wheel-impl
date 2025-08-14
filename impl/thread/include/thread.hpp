#ifndef MY_THREAD_HPP
#define MY_THREAD_HPP

#include <pthread.h>
#include <functional>
#include <memory>
#include <utility>
#include <system_error>
#include <chrono>
#include <ostream>
#include <tuple>
#include <unistd.h>
#include <sched.h>

namespace my {

// 线程ID类
class thread_id {
private:
    pthread_t id_;
    
public:
    thread_id() noexcept : id_(0) {}
    explicit thread_id(pthread_t id) noexcept : id_(id) {}
    
    bool operator==(const thread_id& other) const noexcept {
        return pthread_equal(id_, other.id_) != 0;
    }
    
    bool operator!=(const thread_id& other) const noexcept {
        return !(*this == other);
    }
    
    bool operator<(const thread_id& other) const noexcept {
        return id_ < other.id_;
    }
    
    bool operator<=(const thread_id& other) const noexcept {
        return id_ <= other.id_;
    }
    
    bool operator>(const thread_id& other) const noexcept {
        return id_ > other.id_;
    }
    
    bool operator>=(const thread_id& other) const noexcept {
        return id_ >= other.id_;
    }
    
    friend std::ostream& operator<<(std::ostream& os, const thread_id& id) {
        return os << id.id_;
    }
    
private:
    friend class thread;
    pthread_t native_handle() const noexcept { return id_; }
};

// 线程类
class thread {
public:
    // 类型别名
    using id = thread_id;
    using native_handle_type = pthread_t;
    
    // 构造函数和析构函数
    thread() noexcept = default;
    
    template <typename F, typename... Args>
    explicit thread(F&& f, Args&&... args);
    
    ~thread();
    
    // 拷贝和移动语义
    thread(const thread&) = delete;
    thread& operator=(const thread&) = delete;
    
    thread(thread&& other) noexcept;
    thread& operator=(thread&& other) noexcept;
    
    // 成员函数
    void join();
    void detach();
    bool joinable() const noexcept;
    id get_id() const noexcept;
    native_handle_type native_handle() const noexcept;
    
    void swap(thread& other) noexcept;
    
    // 静态成员函数
    static unsigned int hardware_concurrency() noexcept;
    
private:
    pthread_t thread_id_ = 0;
    bool joinable_ = false;
    
    // 线程启动函数
    static void* thread_func(void* arg);
    
    // 包装函数对象
    class thread_func_base {
    public:
        virtual ~thread_func_base() = default;
        virtual void operator()() = 0;
    };
    
    template <typename F>
    class thread_func_wrapper : public thread_func_base {
    public:
        template <typename U>
        explicit thread_func_wrapper(U&& func) : func_(std::forward<U>(func)) {}
        
        void operator()() override {
            func_();
        }
        
    private:
        F func_;
    };
    
    std::unique_ptr<thread_func_base> func_ptr_;
};

// this_thread 命名空间
namespace this_thread {

thread_id get_id() noexcept;
void yield() noexcept;
void sleep_for(const std::chrono::nanoseconds& sleep_duration);
void sleep_until(const std::chrono::system_clock::time_point& sleep_time);

} // namespace this_thread

// 非成员函数
void swap(thread& t1, thread& t2) noexcept;

// 线程函数实现
template <typename F, typename... Args>
thread::thread(F&& f, Args&&... args) {
    // 创建包装函数对象
    auto func = [f = std::forward<F>(f), args = std::make_tuple(std::forward<Args>(args)...)]() mutable {
        std::apply(f, args);
    };
    
    using FuncType = decltype(func);
    auto wrapper = new thread_func_wrapper<FuncType>(std::move(func));
    func_ptr_ = std::unique_ptr<thread_func_base>(wrapper);
    
    // 创建线程
    int result = pthread_create(&thread_id_, nullptr, &thread::thread_func, wrapper);
    
    if (result != 0) {
        delete wrapper;
        func_ptr_.reset();
        throw std::system_error(result, std::system_category());
    }
    
    joinable_ = true;
}

inline thread::~thread() {
    if (joinable()) {
        std::terminate();
    }
}

inline thread::thread(thread&& other) noexcept
    : thread_id_(other.thread_id_),
      joinable_(other.joinable_),
      func_ptr_(std::move(other.func_ptr_)) {
    other.thread_id_ = 0;
    other.joinable_ = false;
}

inline thread& thread::operator=(thread&& other) noexcept {
    if (this != &other) {
        if (joinable()) {
            std::terminate();
        }
        
        thread_id_ = other.thread_id_;
        joinable_ = other.joinable_;
        func_ptr_ = std::move(other.func_ptr_);
        
        other.thread_id_ = 0;
        other.joinable_ = false;
    }
    return *this;
}

inline void thread::join() {
    if (!joinable()) {
        throw std::system_error(EINVAL, std::system_category());
    }
    
    int result = pthread_join(thread_id_, nullptr);
    if (result != 0) {
        throw std::system_error(result, std::system_category());
    }
    
    thread_id_ = 0;
    joinable_ = false;
}

inline void thread::detach() {
    if (!joinable()) {
        throw std::system_error(EINVAL, std::system_category());
    }
    
    int result = pthread_detach(thread_id_);
    if (result != 0) {
        throw std::system_error(result, std::system_category());
    }
    
    thread_id_ = 0;
    joinable_ = false;
}

inline bool thread::joinable() const noexcept {
    return joinable_;
}

inline thread::id thread::get_id() const noexcept {
    return joinable_ ? id(thread_id_) : id();
}

inline thread::native_handle_type thread::native_handle() const noexcept {
    return thread_id_;
}

inline void thread::swap(thread& other) noexcept {
    using std::swap;
    swap(thread_id_, other.thread_id_);
    swap(joinable_, other.joinable_);
    swap(func_ptr_, other.func_ptr_);
}

inline unsigned int thread::hardware_concurrency() noexcept {
    long nprocs = sysconf(_SC_NPROCESSORS_ONLN);
    return nprocs > 0 ? static_cast<unsigned int>(nprocs) : 1;
}

inline void* thread::thread_func(void* arg) {
    try {
        auto wrapper = static_cast<thread_func_base*>(arg);
        (*wrapper)();
    } catch (...) {
        // 线程函数中的异常会导致程序终止
        std::terminate();
    }
    return nullptr;
}

// this_thread 命名空间实现
namespace this_thread {

inline thread_id get_id() noexcept {
    return thread_id(pthread_self());
}

inline void yield() noexcept {
    sched_yield();
}

inline void sleep_for(const std::chrono::nanoseconds& sleep_duration) {
    if (sleep_duration.count() > 0) {
        timespec req = {
            .tv_sec = static_cast<time_t>(sleep_duration.count() / 1000000000),
            .tv_nsec = static_cast<long>(sleep_duration.count() % 1000000000)
        };
        
        timespec rem;
        while (nanosleep(&req, &rem) == -1 && errno == EINTR) {
            req = rem;
        }
    }
}

inline void sleep_until(const std::chrono::system_clock::time_point& sleep_time) {
    auto now = std::chrono::system_clock::now();
    if (sleep_time > now) {
        sleep_for(sleep_time - now);
    }
}

} // namespace this_thread

// 非成员函数实现
inline void swap(thread& t1, thread& t2) noexcept {
    t1.swap(t2);
}

} // namespace my

#endif // MY_THREAD_HPP
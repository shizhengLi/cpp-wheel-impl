#pragma once

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <atomic>
#include <chrono>
#include <stdexcept>
#include <type_traits>
#include <memory>

namespace impl {

/**
 * @brief 线程池异常类
 */
class thread_pool_exception : public std::runtime_error {
public:
    explicit thread_pool_exception(const std::string& msg) : std::runtime_error(msg) {}
};

/**
 * @brief 任务包装器，支持任意返回类型的任务
 */
class task_wrapper {
public:
    task_wrapper() = default;
    
    template <typename F>
    task_wrapper(F&& f) {
        using Result = typename std::result_of<F()>::type;
        auto task = std::make_shared<std::packaged_task<Result()>>(
            std::forward<F>(f)
        );
        future_ = task->get_future();
        func_ = [task]() {
            (*task)();
        };
    }
    
    void operator()() {
        if (func_) {
            func_();
        }
    }
    
    bool valid() const {
        return future_.valid();
    }
    
    void wait() {
        if (future_.valid()) {
            future_.wait();
        }
    }
    
    template <typename Rep, typename Period>
    std::future_status wait_for(const std::chrono::duration<Rep, Period>& timeout) {
        if (future_.valid()) {
            return future_.wait_for(timeout);
        }
        return std::future_status::ready;
    }
    
private:
    std::function<void()> func_;
    std::future<void> future_;
};

/**
 * @brief 线程池实现
 * 
 * 特点：
 * - 动态线程管理
 * - 任务队列和调度
 * - 支持任意返回类型的任务
 * - 线程安全的任务提交
 * - 优雅关闭和任务等待
 * - 负载均衡和性能优化
 */
class thread_pool {
public:
    /**
     * @brief 构造函数
     * @param thread_count 线程数量（0表示使用硬件并发数）
     * @param max_queue_size 最大队列大小（0表示无限制）
     */
    explicit thread_pool(size_t thread_count = 0, size_t max_queue_size = 0)
        : stop_(false)
        , paused_(false)
        , active_threads_(0)
        , total_tasks_(0)
        , max_queue_size_(max_queue_size) {
        
        if (thread_count == 0) {
            thread_count = std::thread::hardware_concurrency();
            if (thread_count == 0) {
                thread_count = 4; // 默认4个线程
            }
        }
        
        workers_.reserve(thread_count);
        for (size_t i = 0; i < thread_count; ++i) {
            workers_.emplace_back(&thread_pool::worker_thread, this);
        }
    }
    
    /**
     * @brief 析构函数，等待所有任务完成
     */
    ~thread_pool() {
        shutdown();
    }
    
    /**
     * @brief 提交任务到线程池
     * @param f 可调用对象
     * @param args 参数
     * @return 返回任务结果的future
     */
    template <typename F, typename... Args>
    auto submit(F&& f, Args&&... args) 
        -> std::future<typename std::result_of<F(Args...)>::type> {
        
        using ReturnType = typename std::result_of<F(Args...)>::type;
        
        auto task = std::make_shared<std::packaged_task<ReturnType()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );
        
        std::future<ReturnType> result = task->get_future();
        
        {
            std::unique_lock<std::mutex> lock(queue_mutex_);
            
            if (stop_) {
                throw thread_pool_exception("Thread pool is stopped");
            }
            
            if (max_queue_size_ > 0 && task_queue_.size() >= max_queue_size_) {
                throw thread_pool_exception("Task queue is full");
            }
            
            task_queue_.emplace([task]() {
                (*task)();
            });
            
            total_tasks_++;
        }
        
        condition_.notify_one();
        return result;
    }
    
    /**
     * @brief 提交任务，但不关心返回值
     * @param f 可调用对象
     * @param args 参数
     */
    template <typename F, typename... Args>
    void execute(F&& f, Args&&... args) {
        submit(std::forward<F>(f), std::forward<Args>(args)...);
    }
    
    /**
     * @brief 批量提交任务
     * @param tasks 任务列表
     * @return future列表
     */
    template <typename F, typename Container>
    std::vector<std::future<typename std::result_of<F(typename Container::value_type)>::type>>
    submit_batch(F&& f, const Container& container) {
        
        using ReturnType = typename std::result_of<F(typename Container::value_type)>::type;
        std::vector<std::future<ReturnType>> futures;
        
        for (const auto& item : container) {
            futures.push_back(submit(f, item));
        }
        
        return futures;
    }
    
    /**
     * @brief 暂停线程池
     */
    void pause() {
        {
            std::unique_lock<std::mutex> lock(queue_mutex_);
            paused_ = true;
        }
        condition_.notify_all();
    }
    
    /**
     * @brief 恢复线程池
     */
    void resume() {
        {
            std::unique_lock<std::mutex> lock(queue_mutex_);
            paused_ = false;
        }
        condition_.notify_all();
    }
    
    /**
     * @brief 优雅关闭线程池
     * @param wait_for_tasks 是否等待所有任务完成
     */
    void shutdown(bool wait_for_tasks = true) {
        {
            std::unique_lock<std::mutex> lock(queue_mutex_);
            stop_ = true;
            if (!wait_for_tasks) {
                // 清空任务队列
                std::queue<task_wrapper> empty;
                task_queue_.swap(empty);
            }
        }
        
        condition_.notify_all();
        
        // 等待所有工作线程结束
        for (std::thread& worker : workers_) {
            if (worker.joinable()) {
                worker.join();
            }
        }
    }
    
    /**
     * @brief 等待所有任务完成
     */
    void wait_all() {
        while (true) {
            {
                std::unique_lock<std::mutex> lock(queue_mutex_);
                if (task_queue_.empty() && active_threads_ == 0) {
                    break;
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
    
    /**
     * @brief 等待所有任务完成，带超时
     * @param timeout 超时时间
     * @return 是否所有任务都已完成
     */
    template <typename Rep, typename Period>
    bool wait_all_for(const std::chrono::duration<Rep, Period>& timeout) {
        auto start = std::chrono::steady_clock::now();
        
        while (true) {
            {
                std::unique_lock<std::mutex> lock(queue_mutex_);
                if (task_queue_.empty() && active_threads_ == 0) {
                    return true;
                }
            }
            
            auto elapsed = std::chrono::steady_clock::now() - start;
            if (elapsed >= timeout) {
                return false;
            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
    
    /**
     * @brief 获取线程数量
     * @return 线程数量
     */
    size_t thread_count() const {
        return workers_.size();
    }
    
    /**
     * @brief 获取活跃线程数量
     * @return 活跃线程数量
     */
    size_t active_thread_count() const {
        return active_threads_.load();
    }
    
    /**
     * @brief 获取待处理任务数量
     * @return 待处理任务数量
     */
    size_t pending_tasks() const {
        std::unique_lock<std::mutex> lock(const_cast<std::mutex&>(queue_mutex_));
        return task_queue_.size();
    }
    
    /**
     * @brief 获取已处理任务总数
     * @return 已处理任务总数
     */
    size_t total_tasks() const {
        return total_tasks_.load();
    }
    
    /**
     * @brief 检查线程池是否已停止
     * @return 是否已停止
     */
    bool is_stopped() const {
        return stop_;
    }
    
    /**
     * @brief 检查线程池是否已暂停
     * @return 是否已暂停
     */
    bool is_paused() const {
        return paused_;
    }
    
    /**
     * @brief 获取线程池统计信息
     * @return 统计信息字符串
     */
    std::string get_stats() const {
        std::unique_lock<std::mutex> lock(const_cast<std::mutex&>(queue_mutex_));
        return "ThreadPool Stats:\n"
               "  Thread count: " + std::to_string(workers_.size()) + "\n"
               "  Active threads: " + std::to_string(active_threads_.load()) + "\n"
               "  Pending tasks: " + std::to_string(task_queue_.size()) + "\n"
               "  Total tasks processed: " + std::to_string(total_tasks_.load()) + "\n"
               "  Max queue size: " + (max_queue_size_ > 0 ? std::to_string(max_queue_size_) : "unlimited") + "\n"
               "  Status: " + (stop_ ? "Stopped" : (paused_ ? "Paused" : "Running"));
    }
    
    /**
     * @brief 调整线程数量
     * @param new_thread_count 新的线程数量
     */
    void resize(size_t new_thread_count) {
        if (new_thread_count == workers_.size()) {
            return;
        }
        
        {
            std::unique_lock<std::mutex> lock(queue_mutex_);
            if (stop_) {
                return;
            }
        }
        
        if (new_thread_count > workers_.size()) {
            // 增加线程
            size_t to_add = new_thread_count - workers_.size();
            for (size_t i = 0; i < to_add; ++i) {
                workers_.emplace_back(&thread_pool::worker_thread, this);
            }
        } else {
            // 减少线程
            size_t to_remove = workers_.size() - new_thread_count;
            
            // 标记需要停止的线程
            std::vector<std::thread> threads_to_stop;
            {
                std::unique_lock<std::mutex> lock(queue_mutex_);
                for (size_t i = 0; i < to_remove; ++i) {
                    if (!workers_.empty()) {
                        threads_to_stop.push_back(std::move(workers_.back()));
                        workers_.pop_back();
                    }
                }
            }
            
            // 通知所有线程，让它们检查是否应该停止
            condition_.notify_all();
            
            // 等待这些线程结束
            for (auto& thread : threads_to_stop) {
                if (thread.joinable()) {
                    thread.join();
                }
            }
        }
    }
    
    // 禁用拷贝构造和拷贝赋值
    thread_pool(const thread_pool&) = delete;
    thread_pool& operator=(const thread_pool&) = delete;

private:
    /**
     * @brief 工作线程函数
     */
    void worker_thread() {
        while (true) {
            task_wrapper task;
            
            {
                std::unique_lock<std::mutex> lock(queue_mutex_);
                
                // 等待任务或停止信号
                condition_.wait(lock, [this] {
                    return stop_ || (!task_queue_.empty() && !paused_);
                });
                
                if (stop_ && task_queue_.empty()) {
                    return;
                }
                
                if (paused_ || task_queue_.empty()) {
                    continue;
                }
                
                task = std::move(task_queue_.front());
                task_queue_.pop();
                active_threads_++;
            }
            
            // 执行任务
            try {
                task();
            } catch (const std::exception& e) {
                // 记录异常，但不中断线程池运行
                // 在实际应用中，可以添加日志系统
            } catch (...) {
                // 捕获所有异常
            }
            
            {
                std::unique_lock<std::mutex> lock(queue_mutex_);
                active_threads_--;
            }
        }
    }
    
    std::vector<std::thread> workers_;          // 工作线程
    std::queue<task_wrapper> task_queue_;      // 任务队列
    std::mutex queue_mutex_;                   // 队列互斥锁
    std::condition_variable condition_;       // 条件变量
    
    std::atomic<bool> stop_;                  // 停止标志
    std::atomic<bool> paused_;                // 暂停标志
    std::atomic<size_t> active_threads_;      // 活跃线程数
    std::atomic<size_t> total_tasks_;         // 总任务数
    size_t max_queue_size_;                   // 最大队列大小
};

/**
 * @brief 线程池工具函数
 */
namespace thread_pool_utils {

/**
 * @brief 并行执行for循环
 * @param pool 线程池
 * @param start 起始索引
 * @param end 结束索引
 * @param f 要执行的函数
 */
template <typename F>
void parallel_for(thread_pool& pool, size_t start, size_t end, F&& f) {
    const size_t chunk_size = std::max(size_t(1), (end - start) / pool.thread_count());
    
    std::vector<std::future<void>> futures;
    
    for (size_t i = start; i < end; i += chunk_size) {
        size_t chunk_end = std::min(i + chunk_size, end);
        futures.push_back(pool.submit([f, i, chunk_end]() {
            for (size_t j = i; j < chunk_end; ++j) {
                f(j);
            }
        }));
    }
    
    for (auto& future : futures) {
        future.wait();
    }
}

/**
 * @brief 并行执行map操作
 * @param pool 线程池
 * @param container 输入容器
 * @param f 映射函数
 * @return 结果容器
 */
template <typename F, typename Container>
auto parallel_map(thread_pool& pool, const Container& container, F&& f) 
    -> std::vector<typename std::result_of<F(typename Container::value_type)>::type> {
    
    using ResultType = typename std::result_of<F(typename Container::value_type)>::type;
    std::vector<ResultType> results(container.size());
    
    parallel_for(pool, 0, container.size(), [&](size_t i) {
        results[i] = f(container[i]);
    });
    
    return results;
}

/**
 * @brief 并行执行reduce操作
 * @param pool 线程池
 * @param container 输入容器
 * @param f 归约函数
 * @param initial 初始值
 * @return 归约结果
 */
template <typename F, typename Container, typename T>
T parallel_reduce(thread_pool& pool, const Container& container, F&& f, T initial) {
    if (container.empty()) {
        return initial;
    }
    
    const size_t chunk_size = std::max(size_t(1), container.size() / pool.thread_count());
    
    std::vector<std::future<T>> futures;
    
    for (size_t i = 0; i < container.size(); i += chunk_size) {
        size_t chunk_end = std::min(i + chunk_size, container.size());
        futures.push_back(pool.submit([&f, &container, i, chunk_end]() {
            T result = container[i];
            for (size_t j = i + 1; j < chunk_end; ++j) {
                result = f(result, container[j]);
            }
            return result;
        }));
    }
    
    T final_result = initial;
    for (auto& future : futures) {
        final_result = f(final_result, future.get());
    }
    
    return final_result;
}

} // namespace thread_pool_utils

} // namespace impl
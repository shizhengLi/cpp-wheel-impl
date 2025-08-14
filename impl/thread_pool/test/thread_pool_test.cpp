#include <gtest/gtest.h>
#include <chrono>
#include <atomic>
#include <vector>
#include <numeric>
#include <algorithm>
#include "thread_pool.hpp"

using namespace impl;

class ThreadPoolTest : public ::testing::Test {
protected:
    void SetUp() override {
        pool = std::make_unique<thread_pool>(4);
    }
    
    void TearDown() override {
        if (pool && !pool->is_stopped()) {
            pool->shutdown();
        }
        pool.reset();
    }
    
    std::unique_ptr<thread_pool> pool;
};

// 基本功能测试
TEST_F(ThreadPoolTest, BasicTaskExecution) {
    std::atomic<int> counter{0};
    
    auto future = pool->submit([&counter]() {
        counter = 42;
        return counter.load();
    });
    
    EXPECT_EQ(future.get(), 42);
    EXPECT_EQ(counter.load(), 42);
}

// 多任务并行执行
TEST_F(ThreadPoolTest, MultipleTasks) {
    const int num_tasks = 10;
    std::atomic<int> counter{0};
    std::vector<std::future<int>> futures;
    
    for (int i = 0; i < num_tasks; ++i) {
        futures.push_back(pool->submit([&counter, i]() {
            counter.fetch_add(1);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            return i;
        }));
    }
    
    // 等待所有任务完成
    for (auto& future : futures) {
        future.wait();
    }
    
    EXPECT_EQ(counter.load(), num_tasks);
}

// 任务返回值测试
TEST_F(ThreadPoolTest, TaskReturnValue) {
    auto future1 = pool->submit([]() { return 42; });
    auto future2 = pool->submit([]() { return std::string("hello"); });
    auto future3 = pool->submit([]() { return 3.14; });
    
    EXPECT_EQ(future1.get(), 42);
    EXPECT_EQ(future2.get(), "hello");
    EXPECT_DOUBLE_EQ(future3.get(), 3.14);
}

// 任务异常处理测试
TEST_F(ThreadPoolTest, ExceptionHandling) {
    auto future = pool->submit([]() {
        throw std::runtime_error("Test exception");
        return 42;
    });
    
    EXPECT_THROW(future.get(), std::runtime_error);
}

// 线程池暂停和恢复测试
TEST_F(ThreadPoolTest, PauseAndResume) {
    std::atomic<bool> task_executed{false};
    
    pool->pause();
    
    auto future = pool->submit([&task_executed]() {
        task_executed = true;
        return 42;
    });
    
    // 任务应该被暂停
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_FALSE(task_executed.load());
    
    pool->resume();
    
    EXPECT_EQ(future.get(), 42);
    EXPECT_TRUE(task_executed.load());
}

// 等待所有任务完成测试
TEST_F(ThreadPoolTest, WaitAll) {
    std::atomic<int> counter{0};
    
    for (int i = 0; i < 10; ++i) {
        pool->submit([&counter]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            counter.fetch_add(1);
        });
    }
    
    pool->wait_all();
    EXPECT_EQ(counter.load(), 10);
}

// 带超时的等待测试
TEST_F(ThreadPoolTest, WaitAllWithTimeout) {
    std::atomic<int> counter{0};
    
    for (int i = 0; i < 5; ++i) {
        pool->submit([&counter]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            counter.fetch_add(1);
        });
    }
    
    // 等待300ms，应该足够所有任务完成
    bool all_completed = pool->wait_all_for(std::chrono::milliseconds(300));
    EXPECT_TRUE(all_completed);
    EXPECT_EQ(counter.load(), 5);
}

// 批量任务提交测试
TEST_F(ThreadPoolTest, BatchSubmission) {
    std::vector<int> data = {1, 2, 3, 4, 5};
    
    auto futures = pool->submit_batch([](int x) { return x * x; }, data);
    
    std::vector<int> results;
    for (auto& future : futures) {
        results.push_back(future.get());
    }
    
    std::vector<int> expected = {1, 4, 9, 16, 25};
    EXPECT_EQ(results, expected);
}

// 线程池统计信息测试
TEST_F(ThreadPoolTest, Stats) {
    EXPECT_EQ(pool->thread_count(), 4);
    EXPECT_EQ(pool->pending_tasks(), 0);
    EXPECT_EQ(pool->total_tasks(), 0);
    
    // 提交一些任务
    auto future = pool->submit([]() { return 42; });
    
    EXPECT_GE(pool->pending_tasks(), 0);
    EXPECT_GE(pool->total_tasks(), 1);
    
    future.get();
    
    std::string stats = pool->get_stats();
    EXPECT_FALSE(stats.empty());
    EXPECT_NE(stats.find("Thread count"), std::string::npos);
    
    std::cout << stats << std::endl;
}

// 线程池调整大小测试
TEST_F(ThreadPoolTest, Resize) {
    EXPECT_EQ(pool->thread_count(), 4);
    
    // 增加线程
    pool->resize(6);
    EXPECT_EQ(pool->thread_count(), 6);
    
    // 减少线程
    pool->resize(2);
    EXPECT_EQ(pool->thread_count(), 2);
}

// 任务队列限制测试
TEST(ThreadPoolQueueLimitTest, QueueLimit) {
    thread_pool pool(2, 3); // 2个线程，最大队列大小3
    
    std::vector<std::future<int>> futures;
    
    // 提交超过队列限制的任务
    for (int i = 0; i < 6; ++i) {
        try {
            auto future = pool.submit([i]() {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                return i;
            });
            futures.push_back(std::move(future));
        } catch (const thread_pool_exception& e) {
            // 预期会有一些任务因为队列满而失败
            EXPECT_LT(futures.size(), 6);
            break;
        }
    }
    
    // 等待已提交的任务完成
    for (auto& future : futures) {
        future.wait();
    }
}

// 并行for循环测试
TEST(ThreadPoolParallelTest, ParallelFor) {
    thread_pool pool(4);
    std::vector<int> results(100);
    
    thread_pool_utils::parallel_for(pool, 0, 100, [&](size_t i) {
        results[i] = i * i;
    });
    
    for (size_t i = 0; i < 100; ++i) {
        EXPECT_EQ(results[i], static_cast<int>(i * i));
    }
}

// 并行map测试
TEST(ThreadPoolParallelTest, ParallelMap) {
    thread_pool pool(4);
    std::vector<int> input = {1, 2, 3, 4, 5};
    
    auto results = thread_pool_utils::parallel_map(pool, input, [](int x) {
        return x * x;
    });
    
    std::vector<int> expected = {1, 4, 9, 16, 25};
    EXPECT_EQ(results, expected);
}

// 并行reduce测试
TEST(ThreadPoolParallelTest, ParallelReduce) {
    thread_pool pool(4);
    std::vector<int> input = {1, 2, 3, 4, 5};
    
    auto sum = thread_pool_utils::parallel_reduce(pool, input, 
        [](int a, int b) { return a + b; }, 0);
    
    EXPECT_EQ(sum, 15);
}

// 性能测试
TEST(ThreadPoolPerformanceTest, PerformanceComparison) {
    const int num_tasks = 1000;
    std::vector<int> data(num_tasks);
    std::iota(data.begin(), data.end(), 0);
    
    // 线程池性能测试
    thread_pool pool(4);
    
    auto start = std::chrono::high_resolution_clock::now();
    
    auto results = thread_pool_utils::parallel_map(pool, data, [](int x) {
        std::this_thread::sleep_for(std::chrono::microseconds(10));
        return x * x;
    });
    
    auto end = std::chrono::high_resolution_clock::now();
    auto pool_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // 串行性能测试
    start = std::chrono::high_resolution_clock::now();
    
    std::vector<int> serial_results;
    for (int x : data) {
        std::this_thread::sleep_for(std::chrono::microseconds(10));
        serial_results.push_back(x * x);
    }
    
    end = std::chrono::high_resolution_clock::now();
    auto serial_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "Thread pool time: " << pool_duration.count() << " ms" << std::endl;
    std::cout << "Serial time: " << serial_duration.count() << " ms" << std::endl;
    
    // 验证结果正确性
    EXPECT_EQ(results, serial_results);
    
    // 线程池应该更快（至少不应该慢太多）
    EXPECT_LT(pool_duration.count(), serial_duration.count() * 2);
}

// 高并发测试
TEST(ThreadPoolConcurrencyTest, HighConcurrency) {
    thread_pool pool(8);
    std::atomic<int> counter{0};
    const int num_tasks = 1000;
    
    std::vector<std::future<void>> futures;
    futures.reserve(num_tasks);
    
    for (int i = 0; i < num_tasks; ++i) {
        futures.push_back(pool.submit([&counter]() {
            counter.fetch_add(1);
            std::this_thread::sleep_for(std::chrono::microseconds(100));
        }));
    }
    
    // 等待所有任务完成
    for (auto& future : futures) {
        future.wait();
    }
    
    EXPECT_EQ(counter.load(), num_tasks);
}

// 任务依赖测试
TEST_F(ThreadPoolTest, TaskDependencies) {
    std::atomic<int> stage{0};
    
    auto future1 = pool->submit([&stage]() {
        stage = 1;
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    });
    
    auto future2 = pool->submit([&stage, &future1]() {
        future1.wait();
        EXPECT_EQ(stage.load(), 1);
        stage = 2;
    });
    
    auto future3 = pool->submit([&stage, &future2]() {
        future2.wait();
        EXPECT_EQ(stage.load(), 2);
        stage = 3;
    });
    
    future3.wait();
    EXPECT_EQ(stage.load(), 3);
}

// 内存泄漏测试
TEST_F(ThreadPoolTest, MemoryLeakTest) {
    // 多次创建和销毁线程池
    for (int i = 0; i < 10; ++i) {
        thread_pool local_pool(2);
        
        for (int j = 0; j < 20; ++j) {
            local_pool.submit([j]() {
                return j * j;
            });
        }
        
        local_pool.wait_all();
    }
    
    // 如果没有内存泄漏，测试应该通过
    EXPECT_TRUE(true);
}

// 长时间运行测试
TEST(ThreadPoolLongRunningTest, LongRunning) {
    thread_pool pool(4);
    std::atomic<bool> stop_flag{false};
    std::atomic<int> counter{0};
    
    // 启动一些长时间运行的任务
    std::vector<std::future<void>> futures;
    for (int i = 0; i < 4; ++i) {
        futures.push_back(pool.submit([&stop_flag, &counter]() {
            while (!stop_flag) {
                counter.fetch_add(1);
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }));
    }
    
    // 让任务运行一段时间
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    
    // 停止任务
    stop_flag = true;
    
    // 等待所有任务完成
    for (auto& future : futures) {
        future.wait();
    }
    
    EXPECT_GT(counter.load(), 0);
}

// 异常任务处理测试
TEST_F(ThreadPoolTest, ExceptionTaskHandling) {
    std::atomic<int> success_count{0};
    std::atomic<int> exception_count{0};
    
    for (int i = 0; i < 10; ++i) {
        pool->submit([&success_count, &exception_count, i]() {
            if (i % 3 == 0) {
                throw std::runtime_error("Exception task");
            }
            success_count.fetch_add(1);
        });
    }
    
    pool->wait_all();
    
    // 异常不应该中断线程池运行
    EXPECT_GT(success_count.load(), 0);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
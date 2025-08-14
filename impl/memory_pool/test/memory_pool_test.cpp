#include <gtest/gtest.h>
#include <thread>
#include <vector>
#include <atomic>
#include <chrono>
#include "memory_pool.hpp"

using namespace impl;

class MemoryPoolTest : public ::testing::Test {
protected:
    void SetUp() override {
        pool = std::make_unique<memory_pool>(64, 16, 100);
    }
    
    void TearDown() override {
        pool.reset();
    }
    
    std::unique_ptr<memory_pool> pool;
};

// 基本功能测试
TEST_F(MemoryPoolTest, BasicAllocation) {
    void* ptr1 = pool->allocate();
    ASSERT_NE(ptr1, nullptr);
    
    void* ptr2 = pool->allocate();
    ASSERT_NE(ptr2, nullptr);
    ASSERT_NE(ptr1, ptr2);
    
    pool->deallocate(ptr1);
    pool->deallocate(ptr2);
    
    EXPECT_TRUE(pool->empty());
}

// 内存大小测试
TEST_F(MemoryPoolTest, BlockSizeCheck) {
    EXPECT_EQ(pool->block_size(), 64);
    
    void* ptr = pool->allocate();
    EXPECT_NE(ptr, nullptr);
    
    // 分配的指针应该适当对齐
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr) % alignof(std::max_align_t), 0);
    
    pool->deallocate(ptr);
}

// 容量测试
TEST_F(MemoryPoolTest, CapacityChecks) {
    EXPECT_EQ(pool->allocated_count(), 0);
    EXPECT_EQ(pool->free_count(), 16);
    EXPECT_EQ(pool->total_count(), 16);
    
    void* ptr = pool->allocate();
    
    EXPECT_EQ(pool->allocated_count(), 1);
    EXPECT_EQ(pool->free_count(), 15);
    EXPECT_EQ(pool->total_count(), 16);
    
    pool->deallocate(ptr);
    
    EXPECT_EQ(pool->allocated_count(), 0);
    EXPECT_EQ(pool->free_count(), 16);
    EXPECT_EQ(pool->total_count(), 16);
}

// 最大块数限制测试
TEST_F(MemoryPoolTest, MaxBlocksLimit) {
    memory_pool limited_pool(32, 10, 10);
    
    std::vector<void*> ptrs;
    for (int i = 0; i < 10; ++i) {
        void* ptr = limited_pool.allocate();
        ASSERT_NE(ptr, nullptr);
        ptrs.push_back(ptr);
    }
    
    // 尝试分配第11个块应该失败
    EXPECT_THROW(limited_pool.allocate(), memory_pool_exception);
    
    // 释放一些块后应该可以重新分配
    for (int i = 0; i < 5; ++i) {
        limited_pool.deallocate(ptrs[i]);
    }
    
    void* new_ptr = limited_pool.allocate();
    EXPECT_NE(new_ptr, nullptr);
    limited_pool.deallocate(new_ptr);
    
    // 清理剩余的块
    for (size_t i = 5; i < ptrs.size(); ++i) {
        limited_pool.deallocate(ptrs[i]);
    }
}

// 自动扩展测试
TEST_F(MemoryPoolTest, AutoExpansion) {
    std::vector<void*> ptrs;
    
    // 分配超过初始数量的块
    for (int i = 0; i < 50; ++i) {
        void* ptr = pool->allocate();
        ASSERT_NE(ptr, nullptr);
        ptrs.push_back(ptr);
    }
    
    EXPECT_GT(pool->total_count(), 16);
    EXPECT_EQ(pool->allocated_count(), 50);
    
    // 清理所有块
    for (void* ptr : ptrs) {
        pool->deallocate(ptr);
    }
    
    EXPECT_TRUE(pool->empty());
}

// 收缩测试
TEST_F(MemoryPoolTest, ShrinkPool) {
    std::vector<void*> ptrs;
    
    // 分配一些块
    for (int i = 0; i < 20; ++i) {
        ptrs.push_back(pool->allocate());
    }
    
    size_t original_total = pool->total_count();
    
    // 释放所有块
    for (void* ptr : ptrs) {
        pool->deallocate(ptr);
    }
    
    // 收缩池
    pool->shrink(10);
    
    EXPECT_LE(pool->total_count(), original_total);
    EXPECT_GE(pool->free_count(), 10);
}

// 线程安全测试
TEST_F(MemoryPoolTest, ThreadSafety) {
    const int num_threads = 10;
    const int allocations_per_thread = 100;
    std::vector<std::thread> threads;
    std::vector<std::vector<void*>> thread_ptrs(num_threads);
    
    // 多线程分配
    for (int t = 0; t < num_threads; ++t) {
        threads.emplace_back([this, &thread_ptrs, t, allocations_per_thread]() {
            for (int i = 0; i < allocations_per_thread; ++i) {
                void* ptr = pool->allocate();
                thread_ptrs[t].push_back(ptr);
                
                // 随机决定是否立即释放
                if (i % 2 == 0) {
                    pool->deallocate(ptr);
                    thread_ptrs[t].pop_back();
                }
            }
        });
    }
    
    // 等待所有线程完成
    for (auto& thread : threads) {
        thread.join();
    }
    
    // 验证所有分配的块
    size_t total_allocated = 0;
    for (auto& ptrs : thread_ptrs) {
        total_allocated += ptrs.size();
        for (void* ptr : ptrs) {
            pool->deallocate(ptr);
        }
    }
    
    EXPECT_EQ(pool->allocated_count(), 0);
    EXPECT_GT(pool->total_count(), 0);
}

// 性能测试
TEST_F(MemoryPoolTest, PerformanceTest) {
    const int num_iterations = 10000;
    
    // 内存池性能测试
    auto start = std::chrono::high_resolution_clock::now();
    
    std::vector<void*> pool_ptrs;
    for (int i = 0; i < num_iterations; ++i) {
        void* ptr = pool->allocate();
        pool_ptrs.push_back(ptr);
    }
    
    for (void* ptr : pool_ptrs) {
        pool->deallocate(ptr);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto pool_duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    // 标准分配器性能测试
    start = std::chrono::high_resolution_clock::now();
    
    std::vector<void*> standard_ptrs;
    for (int i = 0; i < num_iterations; ++i) {
        void* ptr = ::operator new(64);
        standard_ptrs.push_back(ptr);
    }
    
    for (void* ptr : standard_ptrs) {
        ::operator delete(ptr);
    }
    
    end = std::chrono::high_resolution_clock::now();
    auto standard_duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "Memory pool time: " << pool_duration.count() << " μs" << std::endl;
    std::cout << "Standard allocator time: " << standard_duration.count() << " μs" << std::endl;
    
    // 内存池应该更快（至少不应该慢太多）
    EXPECT_LT(pool_duration.count(), standard_duration.count() * 2);
}

// 内存泄漏测试
TEST_F(MemoryPoolTest, MemoryLeakTest) {
    size_t initial_total = pool->total_count();
    
    {
        std::vector<void*> ptrs;
        for (int i = 0; i < 100; ++i) {
            ptrs.push_back(pool->allocate());
        }
        
        EXPECT_EQ(pool->allocated_count(), 100);
        EXPECT_GT(pool->total_count(), initial_total);
        
        // 作用域结束时，ptrs被自动清理
    }
    
    // 所有块应该被释放
    EXPECT_TRUE(pool->empty());
}

// 分配器测试
TEST_F(MemoryPoolTest, AllocatorTest) {
    memory_pool_allocator<int> alloc(pool.get());
    
    // 分配单个元素
    int* ptr = alloc.allocate(1);
    ASSERT_NE(ptr, nullptr);
    
    alloc.construct(ptr, 42);
    EXPECT_EQ(*ptr, 42);
    
    alloc.destroy(ptr);
    alloc.deallocate(ptr, 1);
    
    // 分配多个元素（应该使用标准分配器）
    int* array = alloc.allocate(10);
    ASSERT_NE(array, nullptr);
    
    for (int i = 0; i < 10; ++i) {
        alloc.construct(&array[i], i);
    }
    
    for (int i = 0; i < 10; ++i) {
        EXPECT_EQ(array[i], i);
        alloc.destroy(&array[i]);
    }
    
    alloc.deallocate(array, 10);
}

// 异常测试
TEST_F(MemoryPoolTest, ExceptionTest) {
    // 零块大小应该抛出异常
    EXPECT_THROW(memory_pool(0), memory_pool_exception);
    
    // 测试内存耗尽
    memory_pool small_pool(32, 5, 5);
    std::vector<void*> ptrs;
    
    for (int i = 0; i < 5; ++i) {
        ptrs.push_back(small_pool.allocate());
    }
    
    EXPECT_THROW(small_pool.allocate(), memory_pool_exception);
    
    // 清理
    for (void* ptr : ptrs) {
        small_pool.deallocate(ptr);
    }
}

// 统计信息测试
TEST_F(MemoryPoolTest, StatsTest) {
    std::string stats = pool->get_stats();
    
    EXPECT_FALSE(stats.empty());
    EXPECT_NE(stats.find("Block size"), std::string::npos);
    EXPECT_NE(stats.find("Allocated blocks"), std::string::npos);
    EXPECT_NE(stats.find("Free blocks"), std::string::npos);
    
    std::cout << stats << std::endl;
}

// 移动语义测试
TEST_F(MemoryPoolTest, MoveSemantics) {
    void* ptr = pool->allocate();
    
    memory_pool moved_pool = std::move(*pool);
    
    // 原池应该为空
    EXPECT_EQ(pool->total_count(), 0);
    
    // 新池应该有原来的内容
    EXPECT_GT(moved_pool.total_count(), 0);
    EXPECT_EQ(moved_pool.allocated_count(), 1);
    
    moved_pool.deallocate(ptr);
    EXPECT_TRUE(moved_pool.empty());
}

// 大块分配测试
TEST(MemoryPoolLargeTest, LargeAllocation) {
    memory_pool large_pool(1024, 100, 1000);
    
    std::vector<void*> ptrs;
    for (int i = 0; i < 500; ++i) {
        void* ptr = large_pool.allocate();
        ASSERT_NE(ptr, nullptr);
        ptrs.push_back(ptr);
    }
    
    EXPECT_EQ(large_pool.allocated_count(), 500);
    
    for (void* ptr : ptrs) {
        large_pool.deallocate(ptr);
    }
    
    EXPECT_TRUE(large_pool.empty());
}

// 不同块大小测试
TEST(MemoryPoolDifferentSizesTest, DifferentBlockSizes) {
    std::vector<size_t> sizes = {8, 16, 32, 64, 128, 256, 512, 1024};
    
    for (size_t size : sizes) {
        memory_pool pool(size, 10);
        
        void* ptr = pool.allocate();
        ASSERT_NE(ptr, nullptr);
        EXPECT_EQ(pool.block_size(), std::max(size, sizeof(void*)));
        
        pool.deallocate(ptr);
        EXPECT_TRUE(pool.empty());
    }
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
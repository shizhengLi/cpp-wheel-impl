#include <gtest/gtest.h>
#include <string>
#include <memory>
#include <algorithm>
#include <initializer_list>
#include "stl/container/unordered_set.hpp"

using namespace stl;

class UnorderedSetTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

// 构造函数测试
TEST_F(UnorderedSetTest, Constructors) {
    // 默认构造函数
    unordered_set<int> set1;
    EXPECT_TRUE(set1.empty());
    EXPECT_EQ(set1.size(), 0);
    EXPECT_GT(set1.bucket_count(), 0);
    
    // 指定桶数构造函数
    unordered_set<int> set2(10);
    EXPECT_TRUE(set2.empty());
    EXPECT_EQ(set2.size(), 0);
    EXPECT_GE(set2.bucket_count(), 10);
    
    // 迭代器构造函数
    std::vector<int> vec = {1, 2, 3, 4, 5};
    unordered_set<int> set3(vec.begin(), vec.end());
    EXPECT_EQ(set3.size(), 5);
    EXPECT_EQ(set3.count(1), 1);
    EXPECT_EQ(set3.count(2), 1);
    EXPECT_EQ(set3.count(3), 1);
    EXPECT_EQ(set3.count(4), 1);
    EXPECT_EQ(set3.count(5), 1);
    
    // 初始化列表构造函数
    unordered_set<int> set4 = {1, 2, 3, 4, 5};
    EXPECT_EQ(set4.size(), 5);
    EXPECT_EQ(set4.count(1), 1);
    EXPECT_EQ(set4.count(2), 1);
    EXPECT_EQ(set4.count(3), 1);
    EXPECT_EQ(set4.count(4), 1);
    EXPECT_EQ(set4.count(5), 1);
    
    // 拷贝构造函数
    unordered_set<int> set5(set4);
    EXPECT_EQ(set5.size(), 5);
    EXPECT_EQ(set5.count(1), 1);
    EXPECT_EQ(set5.count(2), 1);
    EXPECT_EQ(set5.count(3), 1);
    EXPECT_EQ(set5.count(4), 1);
    EXPECT_EQ(set5.count(5), 1);
    
    // 移动构造函数
    unordered_set<int> set6(std::move(set5));
    EXPECT_EQ(set6.size(), 5);
    EXPECT_TRUE(set5.empty());
}

// 赋值运算符测试
TEST_F(UnorderedSetTest, AssignmentOperators) {
    unordered_set<int> set1 = {1, 2, 3};
    
    // 拷贝赋值
    unordered_set<int> set2;
    set2 = set1;
    EXPECT_EQ(set2.size(), 3);
    EXPECT_EQ(set2.count(1), 1);
    EXPECT_EQ(set2.count(2), 1);
    EXPECT_EQ(set2.count(3), 1);
    
    // 移动赋值
    unordered_set<int> set3;
    set3 = std::move(set2);
    EXPECT_EQ(set3.size(), 3);
    EXPECT_TRUE(set2.empty());
    
    // 初始化列表赋值
    unordered_set<int> set4;
    set4 = {4, 5, 6, 7};
    EXPECT_EQ(set4.size(), 4);
    EXPECT_EQ(set4.count(4), 1);
    EXPECT_EQ(set4.count(5), 1);
    EXPECT_EQ(set4.count(6), 1);
    EXPECT_EQ(set4.count(7), 1);
}

// 插入测试
TEST_F(UnorderedSetTest, Insert) {
    unordered_set<int> set;
    
    // 插入单个元素
    auto result1 = set.insert(1);
    EXPECT_TRUE(result1.second);
    EXPECT_EQ(*result1.first, 1);
    EXPECT_EQ(set.size(), 1);
    
    // 插入重复元素
    auto result2 = set.insert(1);
    EXPECT_FALSE(result2.second);
    EXPECT_EQ(*result2.first, 1);
    EXPECT_EQ(set.size(), 1);
    
    // 插入多个元素
    set.insert(2);
    set.insert(3);
    set.insert(4);
    set.insert(5);
    
    EXPECT_EQ(set.size(), 5);
    EXPECT_EQ(set.count(1), 1);
    EXPECT_EQ(set.count(2), 1);
    EXPECT_EQ(set.count(3), 1);
    EXPECT_EQ(set.count(4), 1);
    EXPECT_EQ(set.count(5), 1);
    
    // 使用hint插入
    auto it = set.insert(set.cbegin(), 6);
    EXPECT_EQ(*it, 6);
    EXPECT_EQ(set.size(), 6);
    EXPECT_EQ(set.count(6), 1);
    
    // 迭代器范围插入
    std::vector<int> vec = {7, 8, 9};
    set.insert(vec.begin(), vec.end());
    EXPECT_EQ(set.size(), 9);
    EXPECT_EQ(set.count(7), 1);
    EXPECT_EQ(set.count(8), 1);
    EXPECT_EQ(set.count(9), 1);
    
    // 初始化列表插入
    set.insert({10, 11, 12});
    EXPECT_EQ(set.size(), 12);
    EXPECT_EQ(set.count(10), 1);
    EXPECT_EQ(set.count(11), 1);
    EXPECT_EQ(set.count(12), 1);
}

// emplace测试
TEST_F(UnorderedSetTest, Emplace) {
    unordered_set<std::string> set;
    
    // emplace插入
    auto result1 = set.emplace("hello");
    EXPECT_TRUE(result1.second);
    EXPECT_EQ(*result1.first, "hello");
    EXPECT_EQ(set.size(), 1);
    
    // emplace重复元素
    auto result2 = set.emplace("hello");
    EXPECT_FALSE(result2.second);
    EXPECT_EQ(*result2.first, "hello");
    EXPECT_EQ(set.size(), 1);
    
    // emplace_hint
    auto it = set.emplace_hint(set.cbegin(), "world");
    EXPECT_EQ(*it, "world");
    EXPECT_EQ(set.size(), 2);
    EXPECT_EQ(set.count("world"), 1);
    
    // emplace更多元素
    set.emplace("test");
    set.emplace("unordered");
    set.emplace("set");
    
    EXPECT_EQ(set.size(), 5);
    EXPECT_EQ(set.count("hello"), 1);
    EXPECT_EQ(set.count("world"), 1);
    EXPECT_EQ(set.count("test"), 1);
    EXPECT_EQ(set.count("unordered"), 1);
    EXPECT_EQ(set.count("set"), 1);
}

// 查找测试
TEST_F(UnorderedSetTest, Find) {
    unordered_set<int> set;
    
    // 空集合查找
    auto it1 = set.find(1);
    EXPECT_EQ(it1, set.end());
    
    // 插入元素后查找
    set.insert(1);
    set.insert(2);
    set.insert(3);
    
    auto it2 = set.find(1);
    EXPECT_NE(it2, set.end());
    EXPECT_EQ(*it2, 1);
    
    auto it3 = set.find(2);
    EXPECT_NE(it3, set.end());
    EXPECT_EQ(*it3, 2);
    
    auto it4 = set.find(99);
    EXPECT_EQ(it4, set.end());
}

// count测试
TEST_F(UnorderedSetTest, Count) {
    unordered_set<int> set;
    
    // 空集合计数
    EXPECT_EQ(set.count(1), 0);
    
    // 插入元素后计数
    set.insert(1);
    set.insert(2);
    set.insert(3);
    
    EXPECT_EQ(set.count(1), 1);
    EXPECT_EQ(set.count(2), 1);
    EXPECT_EQ(set.count(3), 1);
    EXPECT_EQ(set.count(99), 0);
}

// equal_range测试
TEST_F(UnorderedSetTest, EqualRange) {
    unordered_set<int> set;
    
    // 空集合范围
    auto range1 = set.equal_range(1);
    EXPECT_EQ(range1.first, set.end());
    EXPECT_EQ(range1.second, set.end());
    
    // 插入元素后范围
    set.insert(1);
    set.insert(2);
    set.insert(3);
    
    auto range2 = set.equal_range(1);
    EXPECT_NE(range2.first, set.end());
    EXPECT_EQ(*range2.first, 1);
    auto next = range2.first;
    ++next;
    EXPECT_EQ(range2.second, next);
    
    auto range3 = set.equal_range(99);
    EXPECT_EQ(range3.first, set.end());
    EXPECT_EQ(range3.second, set.end());
}

// 删除测试
TEST_F(UnorderedSetTest, Erase) {
    unordered_set<int> set;
    
    // 插入元素
    set.insert(1);
    set.insert(2);
    set.insert(3);
    set.insert(4);
    set.insert(5);
    
    EXPECT_EQ(set.size(), 5);
    
    // 通过迭代器删除
    auto it = set.find(3);
    auto next_it = set.erase(it);
    EXPECT_EQ(set.size(), 4);
    EXPECT_EQ(set.count(3), 0);
    EXPECT_NE(next_it, set.end());
    
    // 通过key删除
    size_t count = set.erase(2);
    EXPECT_EQ(count, 1);
    EXPECT_EQ(set.size(), 3);
    EXPECT_EQ(set.count(2), 0);
    
    // 删除不存在的key
    count = set.erase(99);
    EXPECT_EQ(count, 0);
    EXPECT_EQ(set.size(), 3);
    
    // 删除范围 - 当前未实现
    // auto first = set.begin();
    // auto last = set.end();
    // set.erase(first, last);
    // EXPECT_TRUE(set.empty());
}

// clear测试
TEST_F(UnorderedSetTest, Clear) {
    unordered_set<int> set;
    
    // 插入元素
    set.insert(1);
    set.insert(2);
    set.insert(3);
    
    EXPECT_EQ(set.size(), 3);
    
    // 清空集合
    set.clear();
    EXPECT_TRUE(set.empty());
    EXPECT_EQ(set.size(), 0);
    EXPECT_EQ(set.count(1), 0);
    EXPECT_EQ(set.count(2), 0);
    EXPECT_EQ(set.count(3), 0);
}

// 容量测试
TEST_F(UnorderedSetTest, Capacity) {
    unordered_set<int> set;
    
    // 初始状态
    EXPECT_TRUE(set.empty());
    EXPECT_EQ(set.size(), 0);
    EXPECT_GT(set.bucket_count(), 0);
    EXPECT_EQ(set.load_factor(), 0.0f);
    
    // 添加元素
    set.insert(1);
    set.insert(2);
    set.insert(3);
    
    EXPECT_FALSE(set.empty());
    EXPECT_EQ(set.size(), 3);
    EXPECT_GT(set.load_factor(), 0.0f);
    
    // max_size
    EXPECT_GT(set.max_size(), 0);
}

// 桶相关测试
TEST_F(UnorderedSetTest, BucketOperations) {
    unordered_set<int> set;
    
    // 初始桶数
    size_t initial_buckets = set.bucket_count();
    EXPECT_GT(initial_buckets, 0);
    
    // max_bucket_count
    EXPECT_GT(set.max_bucket_count(), 0);
    
    // 添加元素
    set.insert(1);
    set.insert(2);
    set.insert(3);
    
    // 检查负载因子
    float load_factor = set.load_factor();
    EXPECT_GT(load_factor, 0.0f);
    EXPECT_LE(load_factor, 1.0f);
    
    // 检查桶大小
    for (size_t i = 0; i < set.bucket_count(); ++i) {
        size_t bucket_size = set.bucket_size(i);
        EXPECT_GE(bucket_size, 0);
    }
    
    // 检查桶索引
    size_t bucket_idx = set.bucket(1);
    EXPECT_LT(bucket_idx, set.bucket_count());
}

// max_load_factor测试
TEST_F(UnorderedSetTest, MaxLoadFactor) {
    unordered_set<int> set;
    
    // 默认最大负载因子
    float default_max_load = set.max_load_factor();
    EXPECT_GT(default_max_load, 0.0f);
    
    // 设置新的最大负载因子
    set.max_load_factor(0.5f);
    EXPECT_EQ(set.max_load_factor(), 0.5f);
}

// rehash测试
TEST_F(UnorderedSetTest, Rehash) {
    unordered_set<int> set;
    
    // 插入一些元素
    set.insert(1);
    set.insert(2);
    set.insert(3);
    
    [[maybe_unused]] size_t old_bucket_count = set.bucket_count();
    
    // rehash到更大的桶数
    set.rehash(20);
    EXPECT_GE(set.bucket_count(), 20);
    EXPECT_EQ(set.size(), 3);
    EXPECT_EQ(set.count(1), 1);
    EXPECT_EQ(set.count(2), 1);
    EXPECT_EQ(set.count(3), 1);
    
    // rehash到更小的桶数（应该不会减小）
    set.rehash(5);
    EXPECT_GE(set.bucket_count(), 5);
    EXPECT_EQ(set.size(), 3);
}

// reserve测试
TEST_F(UnorderedSetTest, Reserve) {
    unordered_set<int> set;
    
    // reserve空间
    set.reserve(100);
    EXPECT_GE(set.bucket_count(), 100);
    
    // 插入元素
    for (int i = 0; i < 50; ++i) {
        set.insert(i);
    }
    
    EXPECT_EQ(set.size(), 50);
    EXPECT_LE(set.load_factor(), set.max_load_factor());
}

// 迭代器测试
TEST_F(UnorderedSetTest, Iterators) {
    unordered_set<int> set;
    
    // 空集合迭代器
    EXPECT_EQ(set.begin(), set.end());
    EXPECT_EQ(set.cbegin(), set.cend());
    
    // 插入元素
    set.insert(1);
    set.insert(2);
    set.insert(3);
    
    // 非空集合迭代器
    EXPECT_NE(set.begin(), set.end());
    EXPECT_NE(set.cbegin(), set.cend());
    
    // 遍历元素
    std::vector<int> elements;
    for (auto it = set.begin(); it != set.end(); ++it) {
        elements.push_back(*it);
    }
    
    EXPECT_EQ(elements.size(), 3);
    EXPECT_TRUE(std::find(elements.begin(), elements.end(), 1) != elements.end());
    EXPECT_TRUE(std::find(elements.begin(), elements.end(), 2) != elements.end());
    EXPECT_TRUE(std::find(elements.begin(), elements.end(), 3) != elements.end());
    
    // 常量迭代器
    const auto& const_set = set;
    std::vector<int> const_elements;
    for (auto it = const_set.begin(); it != const_set.end(); ++it) {
        const_elements.push_back(*it);
    }
    
    EXPECT_EQ(const_elements.size(), 3);
    
    // 本地迭代器
    for (size_t i = 0; i < set.bucket_count(); ++i) {
        auto local_begin = set.begin(i);
        auto local_end = set.end(i);
        for (auto it = local_begin; it != local_end; ++it) {
            EXPECT_TRUE(set.count(*it) > 0);
        }
    }
}

// swap测试
TEST_F(UnorderedSetTest, Swap) {
    unordered_set<int> set1;
    unordered_set<int> set2;
    
    set1.insert(1);
    set1.insert(2);
    set1.insert(3);
    
    set2.insert(4);
    set2.insert(5);
    
    // 交换
    set1.swap(set2);
    
    EXPECT_EQ(set1.size(), 2);
    EXPECT_EQ(set1.count(4), 1);
    EXPECT_EQ(set1.count(5), 1);
    
    EXPECT_EQ(set2.size(), 3);
    EXPECT_EQ(set2.count(1), 1);
    EXPECT_EQ(set2.count(2), 1);
    EXPECT_EQ(set2.count(3), 1);
}

// 哈希策略测试
TEST_F(UnorderedSetTest, HashPolicy) {
    unordered_set<int> set;
    
    // 获取哈希函数
    auto hash_func = set.hash_function();
    EXPECT_EQ(hash_func(1), hash_func(1));
    
    // 获取相等比较函数
    auto key_eq = set.key_eq();
    EXPECT_TRUE(key_eq(1, 1));
    EXPECT_FALSE(key_eq(1, 2));
}

// 字符串键测试
TEST_F(UnorderedSetTest, StringKeys) {
    unordered_set<std::string> set;
    
    // 插入字符串
    set.insert("hello");
    set.insert("world");
    set.insert("test");
    
    EXPECT_EQ(set.size(), 3);
    EXPECT_EQ(set.count("hello"), 1);
    EXPECT_EQ(set.count("world"), 1);
    EXPECT_EQ(set.count("test"), 1);
    
    // 查找字符串
    auto it = set.find("hello");
    EXPECT_NE(it, set.end());
    EXPECT_EQ(*it, "hello");
    
    // 删除字符串
    set.erase("world");
    EXPECT_EQ(set.size(), 2);
    EXPECT_EQ(set.count("world"), 0);
}

// 自定义哈希和比较函数测试
TEST_F(UnorderedSetTest, CustomHashAndEqual) {
    struct custom_hash {
        size_t operator()(int x) const { return x * 2; }
    };
    
    struct custom_equal {
        bool operator()(int a, int b) const { return a == b; }
    };
    
    unordered_set<int, custom_hash, custom_equal> set;
    
    set.insert(1);
    set.insert(2);
    set.insert(3);
    
    EXPECT_EQ(set.size(), 3);
    EXPECT_EQ(set.count(1), 1);
    EXPECT_EQ(set.count(2), 1);
    EXPECT_EQ(set.count(3), 1);
}

// 自动扩容测试
TEST_F(UnorderedSetTest, AutoRehash) {
    unordered_set<int> set;
    
    size_t initial_buckets = set.bucket_count();
    
    // 插入足够多的元素触发自动扩容
    for (int i = 0; i < 100; ++i) {
        set.insert(i);
    }
    
    // 检查是否发生了扩容
    EXPECT_GT(set.bucket_count(), initial_buckets);
    EXPECT_EQ(set.size(), 100);
    
    // 验证所有元素仍然存在
    for (int i = 0; i < 100; ++i) {
        EXPECT_EQ(set.count(i), 1);
    }
}

// 冲突处理测试
TEST_F(UnorderedSetTest, CollisionHandling) {
    // 使用一个会产生相同哈希值的简单哈希函数
    struct bad_hash {
        size_t operator()([[maybe_unused]] int x) const { return 1; } // 所有键都映射到同一个桶
    };
    
    unordered_set<int, bad_hash> set;
    
    // 插入多个会产生冲突的元素
    set.insert(1);
    set.insert(2);
    set.insert(3);
    set.insert(4);
    set.insert(5);
    
    EXPECT_EQ(set.size(), 5);
    EXPECT_EQ(set.count(1), 1);
    EXPECT_EQ(set.count(2), 1);
    EXPECT_EQ(set.count(3), 1);
    EXPECT_EQ(set.count(4), 1);
    EXPECT_EQ(set.count(5), 1);
}

// 边界情况测试
TEST_F(UnorderedSetTest, EdgeCases) {
    unordered_set<int> set;
    
    // 删除空集合的元素
    size_t count = set.erase(1);
    EXPECT_EQ(count, 0);
    
    // 删除end迭代器
    auto it = set.erase(set.end());
    EXPECT_EQ(it, set.end());
    
    // 清空空集合
    EXPECT_NO_THROW(set.clear());
    
    // rehash空集合
    EXPECT_NO_THROW(set.rehash(10));
    
    // reserve 0
    EXPECT_NO_THROW(set.reserve(0));
}

// 比较运算符测试
TEST_F(UnorderedSetTest, ComparisonOperators) {
    unordered_set<int> set1 = {1, 2, 3};
    unordered_set<int> set2 = {1, 2, 3};
    unordered_set<int> set3 = {1, 2, 4};
    unordered_set<int> set4 = {1, 2};
    unordered_set<int> set5 = {1, 2, 3, 4};
    
    EXPECT_TRUE(set1 == set2);
    EXPECT_FALSE(set1 == set3);
    EXPECT_FALSE(set1 == set4);
    EXPECT_FALSE(set1 == set5);
    
    EXPECT_TRUE(set1 != set3);
    EXPECT_FALSE(set1 != set2);
}

// 内存管理测试
TEST_F(UnorderedSetTest, MemoryManagement) {
    unordered_set<std::unique_ptr<int>> set;
    
    // 插入unique_ptr
    set.insert(std::make_unique<int>(42));
    set.insert(std::make_unique<int>(100));
    
    EXPECT_EQ(set.size(), 2);
    
    // 验证unique_ptr内容
    auto it = set.find(std::make_unique<int>(42));
    EXPECT_NE(it, set.end());
    EXPECT_EQ(**it, 42);
    
    // 移动语义
    auto ptr = std::make_unique<int>(200);
    set.insert(std::move(ptr));
    EXPECT_EQ(ptr.get(), nullptr);
    EXPECT_EQ(set.size(), 3);
}

// 分配器测试
TEST_F(UnorderedSetTest, AllocatorTest) {
    using TestAllocator = stl::allocator<int>;
    unordered_set<int, stl::hash<int>, stl::equal_to<int>, TestAllocator> set;
    
    // 使用自定义分配器
    set.insert(1);
    set.insert(2);
    set.insert(3);
    
    EXPECT_EQ(set.size(), 3);
    EXPECT_EQ(set.count(1), 1);
    EXPECT_EQ(set.count(2), 1);
    EXPECT_EQ(set.count(3), 1);
}

// 大规模数据测试
TEST_F(UnorderedSetTest, LargeScaleTest) {
    unordered_set<int> set;
    
    // 插入大量数据
    const int num_elements = 10000;
    for (int i = 0; i < num_elements; ++i) {
        set.insert(i);
    }
    
    EXPECT_EQ(set.size(), num_elements);
    
    // 验证所有元素都存在
    for (int i = 0; i < num_elements; ++i) {
        EXPECT_EQ(set.count(i), 1);
    }
    
    // 验证不存在的元素
    EXPECT_EQ(set.count(-1), 0);
    EXPECT_EQ(set.count(num_elements), 0);
}

// 重复插入测试
TEST_F(UnorderedSetTest, DuplicateInsertion) {
    unordered_set<int> set;
    
    // 多次插入相同元素
    for (int i = 0; i < 10; ++i) {
        auto result = set.insert(42);
        if (i == 0) {
            EXPECT_TRUE(result.second);
        } else {
            EXPECT_FALSE(result.second);
        }
    }
    
    EXPECT_EQ(set.size(), 1);
    EXPECT_EQ(set.count(42), 1);
}

// 混合操作测试
TEST_F(UnorderedSetTest, MixedOperations) {
    unordered_set<int> set;
    
    // 混合插入、删除、查找操作
    for (int i = 0; i < 100; ++i) {
        set.insert(i);
        EXPECT_EQ(set.count(i), 1);
        
        if (i % 2 == 0) {
            set.erase(i);
            EXPECT_EQ(set.count(i), 0);
        }
    }
    
    EXPECT_EQ(set.size(), 50);
    
    // 验证奇数存在，偶数不存在
    for (int i = 0; i < 100; ++i) {
        if (i % 2 == 1) {
            EXPECT_EQ(set.count(i), 1);
        } else {
            EXPECT_EQ(set.count(i), 0);
        }
    }
}
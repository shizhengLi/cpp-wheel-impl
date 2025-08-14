#include <gtest/gtest.h>
#include <string>
#include <memory>
#include <algorithm>
#include <utility>
#include "stl/hash/hash_table.hpp"
#include "stl/functional.hpp"

using namespace stl;

// 测试用的hash_table派生类，实现get_key方法
template <typename Key, typename Value, typename Hash = stl::hash<Key>, typename KeyEqual = stl::equal_to<Key>, typename Allocator = stl::allocator<Value>>
class test_hash_table : public hash_table<Key, Value, Hash, KeyEqual, Allocator> {
public:
    using base_type = hash_table<Key, Value, Hash, KeyEqual, Allocator>;
    using base_type::base_type;
    
protected:
    const Key& get_key(const Value& value) const override {
        return value; // 对于Value=Key的情况，直接返回value
    }
};

class HashTableTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

// 构造函数测试
TEST_F(HashTableTest, Constructors) {
    // 默认构造函数
    test_hash_table<int, int> table1;
    EXPECT_TRUE(table1.empty());
    EXPECT_EQ(table1.size(), 0);
    EXPECT_GT(table1.bucket_count(), 0);
    
    // 指定桶数构造函数
    test_hash_table<int, int> table2(10);
    EXPECT_TRUE(table2.empty());
    EXPECT_EQ(table2.size(), 0);
    EXPECT_GE(table2.bucket_count(), 10);
    
    // 拷贝构造函数
    table1.insert(1);
    table1.insert(2);
    table1.insert(3);
    
    test_hash_table<int, int> table3(table1);
    EXPECT_EQ(table3.size(), 3);
    EXPECT_EQ(table3.count(1), 1);
    EXPECT_EQ(table3.count(2), 1);
    EXPECT_EQ(table3.count(3), 1);
    
    // 移动构造函数
    test_hash_table<int, int> table4(std::move(table1));
    EXPECT_EQ(table4.size(), 3);
    EXPECT_TRUE(table1.empty());
}

// 赋值运算符测试
TEST_F(HashTableTest, AssignmentOperators) {
    test_hash_table<int, int> table1;
    table1.insert(1);
    table1.insert(2);
    table1.insert(3);
    
    // 拷贝赋值
    test_hash_table<int, int> table2;
    table2 = table1;
    EXPECT_EQ(table2.size(), 3);
    EXPECT_EQ(table2.count(1), 1);
    EXPECT_EQ(table2.count(2), 1);
    EXPECT_EQ(table2.count(3), 1);
    
    // 移动赋值
    test_hash_table<int, int> table3;
    table3 = std::move(table2);
    EXPECT_EQ(table3.size(), 3);
    EXPECT_TRUE(table2.empty());
}

// 插入测试
TEST_F(HashTableTest, Insert) {
    test_hash_table<int, int> table;
    
    // 插入单个元素
    auto result1 = table.insert(1);
    EXPECT_TRUE(result1.second);
    EXPECT_EQ(*result1.first, 1);
    EXPECT_EQ(table.size(), 1);
    
    // 插入重复元素
    auto result2 = table.insert(1);
    EXPECT_FALSE(result2.second);
    EXPECT_EQ(*result2.first, 1);
    EXPECT_EQ(table.size(), 1);
    
    // 插入多个元素
    table.insert(2);
    table.insert(3);
    table.insert(4);
    table.insert(5);
    
    EXPECT_EQ(table.size(), 5);
    EXPECT_EQ(table.count(1), 1);
    EXPECT_EQ(table.count(2), 1);
    EXPECT_EQ(table.count(3), 1);
    EXPECT_EQ(table.count(4), 1);
    EXPECT_EQ(table.count(5), 1);
}

// emplace测试
TEST_F(HashTableTest, Emplace) {
    test_hash_table<std::string, std::string> table;
    
    // emplace插入
    auto result1 = table.emplace("hello");
    EXPECT_TRUE(result1.second);
    EXPECT_EQ(*result1.first, "hello");
    EXPECT_EQ(table.size(), 1);
    
    // emplace重复元素
    auto result2 = table.emplace("hello");
    EXPECT_FALSE(result2.second);
    EXPECT_EQ(*result2.first, "hello");
    EXPECT_EQ(table.size(), 1);
    
    // emplace更多元素
    table.emplace("world");
    table.emplace("test");
    
    EXPECT_EQ(table.size(), 3);
    EXPECT_EQ(table.count("hello"), 1);
    EXPECT_EQ(table.count("world"), 1);
    EXPECT_EQ(table.count("test"), 1);
}

// 查找测试
TEST_F(HashTableTest, Find) {
    test_hash_table<int, int> table;
    
    // 空表查找
    auto it1 = table.find(1);
    EXPECT_EQ(it1, table.end());
    
    // 插入元素后查找
    table.insert(1);
    table.insert(2);
    table.insert(3);
    
    auto it2 = table.find(1);
    EXPECT_NE(it2, table.end());
    EXPECT_EQ(*it2, 1);
    
    auto it3 = table.find(2);
    EXPECT_NE(it3, table.end());
    EXPECT_EQ(*it3, 2);
    
    auto it4 = table.find(99);
    EXPECT_EQ(it4, table.end());
}

// count测试
TEST_F(HashTableTest, Count) {
    test_hash_table<int, int> table;
    
    // 空表计数
    EXPECT_EQ(table.count(1), 0);
    
    // 插入元素后计数
    table.insert(1);
    table.insert(2);
    table.insert(3);
    
    EXPECT_EQ(table.count(1), 1);
    EXPECT_EQ(table.count(2), 1);
    EXPECT_EQ(table.count(3), 1);
    EXPECT_EQ(table.count(99), 0);
}

// equal_range测试
TEST_F(HashTableTest, EqualRange) {
    test_hash_table<int, int> table;
    
    // 空表范围
    auto range1 = table.equal_range(1);
    EXPECT_EQ(range1.first, table.end());
    EXPECT_EQ(range1.second, table.end());
    
    // 插入元素后范围
    table.insert(1);
    table.insert(2);
    table.insert(3);
    
    auto range2 = table.equal_range(1);
    EXPECT_NE(range2.first, table.end());
    EXPECT_EQ(*range2.first, 1);
    auto next = range2.first;
    ++next;
    EXPECT_EQ(range2.second, next);
    
    auto range3 = table.equal_range(99);
    EXPECT_EQ(range3.first, table.end());
    EXPECT_EQ(range3.second, table.end());
}

// 删除测试
TEST_F(HashTableTest, Erase) {
    test_hash_table<int, int> table;
    
    // 插入元素
    table.insert(1);
    table.insert(2);
    table.insert(3);
    table.insert(4);
    table.insert(5);
    
    EXPECT_EQ(table.size(), 5);
    
    // 通过迭代器删除
    auto it = table.find(3);
    auto next_it = table.erase(it);
    EXPECT_EQ(table.size(), 4);
    EXPECT_EQ(table.count(3), 0);
    EXPECT_NE(next_it, table.end());
    
    // 通过key删除
    size_t count = table.erase(2);
    EXPECT_EQ(count, 1);
    EXPECT_EQ(table.size(), 3);
    EXPECT_EQ(table.count(2), 0);
    
    // 删除不存在的key
    count = table.erase(99);
    EXPECT_EQ(count, 0);
    EXPECT_EQ(table.size(), 3);
}

// clear测试
TEST_F(HashTableTest, Clear) {
    test_hash_table<int, int> table;
    
    // 插入元素
    table.insert(1);
    table.insert(2);
    table.insert(3);
    
    EXPECT_EQ(table.size(), 3);
    
    // 清空表
    table.clear();
    EXPECT_TRUE(table.empty());
    EXPECT_EQ(table.size(), 0);
    EXPECT_EQ(table.count(1), 0);
    EXPECT_EQ(table.count(2), 0);
    EXPECT_EQ(table.count(3), 0);
}

// 容量测试
TEST_F(HashTableTest, Capacity) {
    test_hash_table<int, int> table;
    
    // 初始状态
    EXPECT_TRUE(table.empty());
    EXPECT_EQ(table.size(), 0);
    EXPECT_GT(table.bucket_count(), 0);
    EXPECT_EQ(table.load_factor(), 0.0f);
    
    // 添加元素
    table.insert(1);
    table.insert(2);
    table.insert(3);
    
    EXPECT_FALSE(table.empty());
    EXPECT_EQ(table.size(), 3);
    EXPECT_GT(table.load_factor(), 0.0f);
    
    // max_size
    EXPECT_GT(table.max_size(), 0);
}

// 桶相关测试
TEST_F(HashTableTest, BucketOperations) {
    test_hash_table<int, int> table;
    
    // 初始桶数
    size_t initial_buckets = table.bucket_count();
    EXPECT_GT(initial_buckets, 0);
    
    // max_bucket_count
    EXPECT_GT(table.max_bucket_count(), 0);
    
    // 添加元素
    table.insert(1);
    table.insert(2);
    table.insert(3);
    
    // 检查负载因子
    float load_factor = table.load_factor();
    EXPECT_GT(load_factor, 0.0f);
    EXPECT_LE(load_factor, 1.0f);
}

// max_load_factor测试
TEST_F(HashTableTest, MaxLoadFactor) {
    test_hash_table<int, int> table;
    
    // 默认最大负载因子
    float default_max_load = table.max_load_factor();
    EXPECT_GT(default_max_load, 0.0f);
    
    // 设置新的最大负载因子
    table.max_load_factor(0.5f);
    EXPECT_EQ(table.max_load_factor(), 0.5f);
}

// rehash测试
TEST_F(HashTableTest, Rehash) {
    test_hash_table<int, int> table;
    
    // 插入一些元素
    table.insert(1);
    table.insert(2);
    table.insert(3);
    
    size_t old_bucket_count = table.bucket_count();
    
    // rehash到更大的桶数
    table.rehash(20);
    EXPECT_GE(table.bucket_count(), 20);
    EXPECT_EQ(table.size(), 3);
    EXPECT_EQ(table.count(1), 1);
    EXPECT_EQ(table.count(2), 1);
    EXPECT_EQ(table.count(3), 1);
    
    // rehash到更小的桶数（应该不会减小）
    table.rehash(5);
    EXPECT_GE(table.bucket_count(), 5);
    EXPECT_EQ(table.size(), 3);
}

// reserve测试
TEST_F(HashTableTest, Reserve) {
    test_hash_table<int, int> table;
    
    // reserve空间
    table.reserve(100);
    EXPECT_GE(table.bucket_count(), 100);
    
    // 插入元素
    for (int i = 0; i < 50; ++i) {
        table.insert(i);
    }
    
    EXPECT_EQ(table.size(), 50);
    EXPECT_LE(table.load_factor(), table.max_load_factor());
}

// 迭代器测试
TEST_F(HashTableTest, Iterators) {
    test_hash_table<int, int> table;
    
    // 空表迭代器
    EXPECT_EQ(table.begin(), table.end());
    EXPECT_EQ(table.cbegin(), table.cend());
    
    // 插入元素
    table.insert(1);
    table.insert(2);
    table.insert(3);
    
    // 非空表迭代器
    EXPECT_NE(table.begin(), table.end());
    EXPECT_NE(table.cbegin(), table.cend());
    
    // 遍历元素
    std::vector<int> elements;
    for (auto it = table.begin(); it != table.end(); ++it) {
        elements.push_back(*it);
    }
    
    EXPECT_EQ(elements.size(), 3);
    EXPECT_TRUE(std::find(elements.begin(), elements.end(), 1) != elements.end());
    EXPECT_TRUE(std::find(elements.begin(), elements.end(), 2) != elements.end());
    EXPECT_TRUE(std::find(elements.begin(), elements.end(), 3) != elements.end());
    
    // 常量迭代器
    const auto& const_table = table;
    std::vector<int> const_elements;
    for (auto it = const_table.begin(); it != const_table.end(); ++it) {
        const_elements.push_back(*it);
    }
    
    EXPECT_EQ(const_elements.size(), 3);
}

// swap测试
TEST_F(HashTableTest, Swap) {
    test_hash_table<int, int> table1;
    test_hash_table<int, int> table2;
    
    table1.insert(1);
    table1.insert(2);
    table1.insert(3);
    
    table2.insert(4);
    table2.insert(5);
    
    // 交换
    table1.swap(table2);
    
    EXPECT_EQ(table1.size(), 2);
    EXPECT_EQ(table1.count(4), 1);
    EXPECT_EQ(table1.count(5), 1);
    
    EXPECT_EQ(table2.size(), 3);
    EXPECT_EQ(table2.count(1), 1);
    EXPECT_EQ(table2.count(2), 1);
    EXPECT_EQ(table2.count(3), 1);
}

// 哈希策略测试
TEST_F(HashTableTest, HashPolicy) {
    test_hash_table<int, int> table;
    
    // 获取哈希函数
    auto hash_func = table.hash_function();
    EXPECT_EQ(hash_func(1), hash_func(1));
    
    // 获取相等比较函数
    auto key_eq = table.key_eq();
    EXPECT_TRUE(key_eq(1, 1));
    EXPECT_FALSE(key_eq(1, 2));
}

// 字符串键测试
TEST_F(HashTableTest, StringKeys) {
    test_hash_table<std::string, std::string> table;
    
    // 插入字符串键
    table.insert("hello");
    table.insert("world");
    table.insert("test");
    
    EXPECT_EQ(table.size(), 3);
    EXPECT_EQ(table.count("hello"), 1);
    EXPECT_EQ(table.count("world"), 1);
    EXPECT_EQ(table.count("test"), 1);
    
    // 查找字符串
    auto it = table.find("hello");
    EXPECT_NE(it, table.end());
    EXPECT_EQ(*it, "hello");
    
    // 删除字符串
    table.erase("world");
    EXPECT_EQ(table.size(), 2);
    EXPECT_EQ(table.count("world"), 0);
}

// 自动扩容测试
TEST_F(HashTableTest, AutoRehash) {
    test_hash_table<int, int> table;
    
    size_t initial_buckets = table.bucket_count();
    
    // 插入足够多的元素触发自动扩容
    for (int i = 0; i < 100; ++i) {
        table.insert(i);
    }
    
    // 检查是否发生了扩容
    EXPECT_GT(table.bucket_count(), initial_buckets);
    EXPECT_EQ(table.size(), 100);
    
    // 验证所有元素仍然存在
    for (int i = 0; i < 100; ++i) {
        EXPECT_EQ(table.count(i), 1);
    }
}

// 冲突处理测试
TEST_F(HashTableTest, CollisionHandling) {
    // 使用一个会产生相同哈希值的简单哈希函数
    struct bad_hash {
        size_t operator()(int x) const { return 1; } // 所有键都映射到同一个桶
    };
    
    test_hash_table<int, int, bad_hash> table;
    
    // 插入多个会产生冲突的元素
    table.insert(1);
    table.insert(2);
    table.insert(3);
    table.insert(4);
    table.insert(5);
    
    EXPECT_EQ(table.size(), 5);
    EXPECT_EQ(table.count(1), 1);
    EXPECT_EQ(table.count(2), 1);
    EXPECT_EQ(table.count(3), 1);
    EXPECT_EQ(table.count(4), 1);
    EXPECT_EQ(table.count(5), 1);
}

// 边界情况测试
TEST_F(HashTableTest, EdgeCases) {
    test_hash_table<int, int> table;
    
    // 删除空表的元素
    size_t count = table.erase(1);
    EXPECT_EQ(count, 0);
    
    // 删除end迭代器
    auto it = table.erase(table.end());
    EXPECT_EQ(it, table.end());
    
    // 清空空表
    EXPECT_NO_THROW(table.clear());
    
    // rehash空表
    EXPECT_NO_THROW(table.rehash(10));
    
    // reserve 0
    EXPECT_NO_THROW(table.reserve(0));
}

// 内存管理测试
TEST_F(HashTableTest, MemoryManagement) {
    test_hash_table<std::unique_ptr<int>, std::unique_ptr<int>> table;
    
    // 插入unique_ptr
    table.insert(std::make_unique<int>(42));
    table.insert(std::make_unique<int>(100));
    
    EXPECT_EQ(table.size(), 2);
    
    // 验证unique_ptr内容
    auto it = table.find(std::make_unique<int>(42));
    EXPECT_NE(it, table.end());
    EXPECT_EQ(**it, 42);
    
    // 移动语义
    auto ptr = std::make_unique<int>(200);
    table.insert(std::move(ptr));
    EXPECT_EQ(ptr.get(), nullptr);
    EXPECT_EQ(table.size(), 3);
}

// 分配器测试
TEST_F(HashTableTest, AllocatorTest) {
    using TestAllocator = stl::allocator<int>;
    test_hash_table<int, int, stl::hash<int>, stl::equal_to<int>, TestAllocator> table;
    
    // 使用自定义分配器
    table.insert(1);
    table.insert(2);
    table.insert(3);
    
    EXPECT_EQ(table.size(), 3);
    EXPECT_EQ(table.count(1), 1);
    EXPECT_EQ(table.count(2), 1);
    EXPECT_EQ(table.count(3), 1);
}

// 大规模数据测试
TEST_F(HashTableTest, LargeScaleTest) {
    test_hash_table<int, int> table;
    
    // 插入大量数据
    const int num_elements = 10000;
    for (int i = 0; i < num_elements; ++i) {
        table.insert(i);
    }
    
    EXPECT_EQ(table.size(), num_elements);
    
    // 验证所有元素都存在
    for (int i = 0; i < num_elements; ++i) {
        EXPECT_EQ(table.count(i), 1);
    }
    
    // 验证不存在的元素
    EXPECT_EQ(table.count(-1), 0);
    EXPECT_EQ(table.count(num_elements), 0);
}
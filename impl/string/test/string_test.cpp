#include <gtest/gtest.h>
#include <memory>
#include <type_traits>

#include "string.hpp"

// 测试基本构造和析构
TEST(StringTest, BasicConstruction) {
    my::string s1;
    EXPECT_TRUE(s1.empty());
    EXPECT_EQ(s1.size(), 0);
    EXPECT_GE(s1.capacity(), 0);  // SSO可能有预分配容量
    
    my::string s2("Hello");
    EXPECT_FALSE(s2.empty());
    EXPECT_EQ(s2.size(), 5);
    EXPECT_EQ(s2, "Hello");
    
    my::string s3(s2);
    EXPECT_EQ(s3, "Hello");
    EXPECT_EQ(s3.size(), 5);
}

// 测试移动构造
TEST(StringTest, MoveConstruction) {
    my::string s1("Hello World");
    my::string s2 = std::move(s1);
    
    EXPECT_EQ(s2, "Hello World");
    EXPECT_TRUE(s1.empty());  // s1应该被清空
}

// 测试移动赋值
TEST(StringTest, MoveAssignment) {
    my::string s1("Hello");
    my::string s2("World");
    
    s2 = std::move(s1);
    EXPECT_EQ(s2, "Hello");
    EXPECT_TRUE(s1.empty());  // s1应该被清空
}

// 测试拷贝构造
TEST(StringTest, CopyConstruction) {
    my::string s1("Hello World");
    my::string s2(s1);
    
    EXPECT_EQ(s2, "Hello World");
    EXPECT_EQ(s1, "Hello World");  // s1应该保持不变
}

// 测试拷贝赋值
TEST(StringTest, CopyAssignment) {
    my::string s1("Hello");
    my::string s2("World");
    
    s2 = s1;
    EXPECT_EQ(s2, "Hello");
    EXPECT_EQ(s1, "Hello");  // s1应该保持不变
}

// 测试容量管理
TEST(StringTest, CapacityManagement) {
    my::string s;
    EXPECT_GE(s.capacity(), 0);  // SSO可能有预分配容量
    
    s.reserve(100);
    EXPECT_GE(s.capacity(), 100);
    EXPECT_EQ(s.size(), 0);
    
    s = "Hello";
    EXPECT_EQ(s.size(), 5);
    EXPECT_GE(s.capacity(), 5);
    
    s.shrink_to_fit();
    EXPECT_LE(s.capacity(), 15);  // SSO可能不会收缩容量
}

// 测试resize操作
TEST(StringTest, ResizeOperation) {
    my::string s("Hello");
    
    s.resize(10, 'x');
    EXPECT_EQ(s.size(), 10);
    EXPECT_EQ(s.substr(0, 5), "Hello");
    EXPECT_EQ(s.substr(5, 5), "xxxxx");
    
    s.resize(3);
    EXPECT_EQ(s.size(), 3);
    EXPECT_EQ(s, "Hel");
}

// 测试append操作
TEST(StringTest, AppendOperation) {
    my::string s("Hello");
    
    s.append(" World");
    EXPECT_EQ(s, "Hello World");
    
    s.append(3, '!');
    EXPECT_EQ(s, "Hello World!!!");
    
    my::string s2(" ABC");
    s.append(s2);
    EXPECT_EQ(s, "Hello World!!! ABC");
}

// 测试substr操作
TEST(StringTest, SubstrOperation) {
    my::string s("Hello World");
    
    EXPECT_EQ(s.substr(0, 5), "Hello");
    EXPECT_EQ(s.substr(6, 5), "World");
    EXPECT_EQ(s.substr(6), "World");
    EXPECT_EQ(s.substr(0), "Hello World");
}

// 测试find操作
TEST(StringTest, FindOperation) {
    my::string s("Hello World Hello");
    
    EXPECT_EQ(s.find("Hello"), 0);
    EXPECT_EQ(s.find("World"), 6);
    EXPECT_EQ(s.find("Hello", 1), 12);
    EXPECT_EQ(s.find("Notfound"), my::string::npos);
}

// 测试rfind操作
TEST(StringTest, RfindOperation) {
    my::string s("Hello World Hello");
    
    EXPECT_EQ(s.rfind("Hello"), 12);
    EXPECT_EQ(s.rfind("World"), 6);
    EXPECT_EQ(s.rfind("Notfound"), my::string::npos);
}

// 测试比较操作
TEST(StringTest, ComparisonOperations) {
    my::string s1("Hello");
    my::string s2("Hello");
    my::string s3("World");
    
    EXPECT_EQ(s1, s2);
    EXPECT_NE(s1, s3);
    EXPECT_LT(s1, s3);
    EXPECT_GT(s3, s1);
    EXPECT_LE(s1, s2);
    EXPECT_GE(s1, s2);
}

// 测试连接操作
TEST(StringTest, Concatenation) {
    my::string s1("Hello");
    my::string s2(" World");
    
    EXPECT_EQ(s1 + s2, "Hello World");
    EXPECT_EQ(s1 + " There", "Hello There");
    EXPECT_EQ("Say " + s1, "Say Hello");
    
    s1 += s2;
    EXPECT_EQ(s1, "Hello World");
}

// 测试访问操作
TEST(StringTest, AccessOperations) {
    my::string s("Hello");
    
    EXPECT_EQ(s[0], 'H');
    EXPECT_EQ(s[4], 'o');
    
    s[0] = 'h';
    EXPECT_EQ(s, "hello");
    
    EXPECT_EQ(s.at(1), 'e');
    EXPECT_THROW(s.at(10), std::out_of_range);
}

// 测试迭代器
TEST(StringTest, Iterators) {
    my::string s("Hello");
    
    // 前向迭代器
    std::string result;
    for (auto it = s.begin(); it != s.end(); ++it) {
        result += *it;
    }
    EXPECT_EQ(result, "Hello");
    
    // 反向迭代器
    std::string reverse_result;
    for (auto it = s.rbegin(); it != s.rend(); ++it) {
        reverse_result += *it;
    }
    EXPECT_EQ(reverse_result, "olleH");
}

// 测试c_str操作
TEST(StringTest, CStrOperation) {
    my::string s("Hello");
    
    EXPECT_STREQ(s.c_str(), "Hello");
    EXPECT_EQ(s.data(), s.c_str());
}

// 测试clear操作
TEST(StringTest, ClearOperation) {
    my::string s("Hello World");
    
    s.clear();
    EXPECT_TRUE(s.empty());
    EXPECT_EQ(s.size(), 0);
    EXPECT_GT(s.capacity(), 0);  // 容量可能保持不变
}

// 测试swap操作
TEST(StringTest, SwapOperation) {
    my::string s1("Hello");
    my::string s2("World");
    
    s1.swap(s2);
    EXPECT_EQ(s1, "World");
    EXPECT_EQ(s2, "Hello");
}

// 测试插入操作
TEST(StringTest, InsertOperation) {
    my::string s("Hello World");
    
    s.insert(5, " Beautiful");
    EXPECT_EQ(s, "Hello Beautiful World");
    
    s.insert(0, "Say ");
    EXPECT_EQ(s, "Say Hello Beautiful World");
}

// 测试删除操作
TEST(StringTest, EraseOperation) {
    my::string s("Hello Beautiful World");
    
    s.erase(6, 10);  // 删除"Beautiful "
    EXPECT_EQ(s, "Hello World");
    
    s.erase(5);  // 删除从位置5开始的所有字符
    EXPECT_EQ(s, "Hello");
}

// 测试替换操作
TEST(StringTest, ReplaceOperation) {
    my::string s("Hello World");
    
    s.replace(6, 5, "C++");
    EXPECT_EQ(s, "Hello C++");
    
    s.replace(0, 5, "Hi");
    EXPECT_EQ(s, "Hi C++");
}

// 测试边界条件
TEST(StringTest, EdgeCases) {
    my::string s;
    
    // 空字符串操作
    EXPECT_TRUE(s.empty());
    EXPECT_EQ(s.size(), 0);
    EXPECT_EQ(s.c_str(), std::string(""));
    
    // 大字符串操作
    s = my::string(1000, 'a');
    EXPECT_EQ(s.size(), 1000);
    EXPECT_EQ(s[0], 'a');
    EXPECT_EQ(s[999], 'a');
    
    // 特殊字符
    s = "Hello\tWorld\n";
    EXPECT_EQ(s, "Hello\tWorld\n");
}

// 测试异常安全
TEST(StringTest, ExceptionSafety) {
    my::string s("Hello");
    
    // 测试at方法的异常
    EXPECT_THROW(s.at(10), std::out_of_range);
    
    // 测试reserve的异常安全
    try {
        s.reserve(SIZE_MAX);
    } catch (const std::bad_alloc&) {
        // 预期的异常
    }
}

// 测试类型特征
TEST(StringTest, TypeTraits) {
    EXPECT_TRUE(std::is_copy_constructible_v<my::string>);
    EXPECT_TRUE(std::is_copy_assignable_v<my::string>);
    EXPECT_TRUE(std::is_move_constructible_v<my::string>);
    EXPECT_TRUE(std::is_move_assignable_v<my::string>);
}

// 测试性能相关
TEST(StringTest, PerformanceRelated) {
    my::string s;
    
    // 测试小字符串优化
    s = "small";
    EXPECT_EQ(s, "small");
    
    // 测试大字符串
    s = "This is a very long string that should trigger dynamic allocation";
    EXPECT_GT(s.size(), 50);
    
    // 测试容量增长策略
    size_t old_capacity = s.capacity();
    s += " and some more text";
    EXPECT_GE(s.capacity(), old_capacity);
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
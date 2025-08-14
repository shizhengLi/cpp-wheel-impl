#include <gtest/gtest.h>
#include <string>
#include <vector>
#include <memory>
#include <type_traits>

#include "../include/variant.hpp"

// 测试基本构造和访问
TEST(VariantTest, BasicConstructionAndAccess) {
    my::variant<int, std::string, double> v1(42);
    
    EXPECT_EQ(v1.index(), 0);
    EXPECT_TRUE(my::holds_alternative<int>(v1));
    EXPECT_EQ(my::get<int>(v1), 42);
    
    // 测试字符串类型
    my::variant<int, std::string, double> v2(std::string("hello"));
    EXPECT_EQ(v2.index(), 1);
    EXPECT_TRUE(my::holds_alternative<std::string>(v2));
    EXPECT_EQ(my::get<std::string>(v2), "hello");
    
    // 测试double类型
    my::variant<int, std::string, double> v3(3.14);
    EXPECT_EQ(v3.index(), 2);
    EXPECT_TRUE(my::holds_alternative<double>(v3));
    EXPECT_DOUBLE_EQ(my::get<double>(v3), 3.14);
}

// 测试默认构造
TEST(VariantTest, DefaultConstruction) {
    my::variant<int, std::string> v;
    EXPECT_EQ(v.index(), 0);
    EXPECT_TRUE(my::holds_alternative<int>(v));
    EXPECT_EQ(my::get<int>(v), 0);
}

// 测试拷贝构造
TEST(VariantTest, CopyConstruction) {
    my::variant<int, std::string> v1(42);
    my::variant<int, std::string> v2(v1);
    
    EXPECT_EQ(v2.index(), 0);
    EXPECT_TRUE(my::holds_alternative<int>(v2));
    EXPECT_EQ(my::get<int>(v2), 42);
}

// 测试移动构造
TEST(VariantTest, MoveConstruction) {
    my::variant<std::unique_ptr<int>, std::string> v1(std::make_unique<int>(42));
    my::variant<std::unique_ptr<int>, std::string> v2(std::move(v1));
    
    EXPECT_EQ(v2.index(), 0);
    EXPECT_TRUE(my::holds_alternative<std::unique_ptr<int>>(v2));
    EXPECT_EQ(*my::get<std::unique_ptr<int>>(v2), 42);
}

// 测试拷贝赋值
TEST(VariantTest, CopyAssignment) {
    my::variant<int, std::string> v1(42);
    my::variant<int, std::string> v2;
    
    v2 = v1;
    EXPECT_EQ(v2.index(), 0);
    EXPECT_TRUE(my::holds_alternative<int>(v2));
    EXPECT_EQ(my::get<int>(v2), 42);
}

// 测试移动赋值
TEST(VariantTest, MoveAssignment) {
    my::variant<std::unique_ptr<int>, std::string> v1(std::make_unique<int>(42));
    my::variant<std::unique_ptr<int>, std::string> v2;
    
    v2 = std::move(v1);
    EXPECT_EQ(v2.index(), 0);
    EXPECT_TRUE(my::holds_alternative<std::unique_ptr<int>>(v2));
    EXPECT_EQ(*my::get<std::unique_ptr<int>>(v2), 42);
}

// 测试值赋值
TEST(VariantTest, ValueAssignment) {
    my::variant<int, std::string> v;
    
    v = 42;
    EXPECT_EQ(v.index(), 0);
    EXPECT_TRUE(my::holds_alternative<int>(v));
    EXPECT_EQ(my::get<int>(v), 42);
    
    v = std::string("hello");
    EXPECT_EQ(v.index(), 1);
    EXPECT_TRUE(my::holds_alternative<std::string>(v));
    EXPECT_EQ(my::get<std::string>(v), "hello");
}

// 测试holds_alternative
TEST(VariantTest, HoldsAlternative) {
    my::variant<int, std::string> v(42);
    
    EXPECT_TRUE(my::holds_alternative<int>(v));
    EXPECT_FALSE(my::holds_alternative<std::string>(v));
    
    v = std::string("hello");
    EXPECT_FALSE(my::holds_alternative<int>(v));
    EXPECT_TRUE(my::holds_alternative<std::string>(v));
}

// 测试get异常
TEST(VariantTest, GetException) {
    my::variant<int, std::string> v(42);
    
    EXPECT_THROW(my::get<std::string>(v), my::bad_variant_access);
    
    v = std::string("hello");
    EXPECT_THROW(my::get<int>(v), my::bad_variant_access);
}

// 测试emplace
TEST(VariantTest, Emplace) {
    my::variant<int, std::string, std::vector<int>> v;
    
    // emplace int
    v.emplace<int>(42);
    EXPECT_EQ(v.index(), 0);
    EXPECT_TRUE(my::holds_alternative<int>(v));
    EXPECT_EQ(my::get<int>(v), 42);
    
    // emplace string
    v.emplace<std::string>("hello");
    EXPECT_EQ(v.index(), 1);
    EXPECT_TRUE(my::holds_alternative<std::string>(v));
    EXPECT_EQ(my::get<std::string>(v), "hello");
    
    // emplace vector
    v.emplace<std::vector<int>>(std::vector<int>{1, 2, 3});
    EXPECT_EQ(v.index(), 2);
    EXPECT_TRUE(my::holds_alternative<std::vector<int>>(v));
    EXPECT_EQ(my::get<std::vector<int>>(v).size(), 3);
}

// 测试按索引emplace
TEST(VariantTest, EmplaceByIndex) {
    my::variant<int, std::string, double> v;
    
    v.emplace<0>(42);
    EXPECT_EQ(v.index(), 0);
    EXPECT_EQ(my::get<0>(v), 42);
    
    v.emplace<1>("hello");
    EXPECT_EQ(v.index(), 1);
    EXPECT_EQ(my::get<1>(v), "hello");
    
    v.emplace<2>(3.14);
    EXPECT_EQ(v.index(), 2);
    EXPECT_DOUBLE_EQ(my::get<2>(v), 3.14);
}

// 测试swap
TEST(VariantTest, Swap) {
    my::variant<int, std::string> v1(42);
    my::variant<int, std::string> v2(std::string("hello"));
    
    v1.swap(v2);
    
    EXPECT_EQ(v1.index(), 1);
    EXPECT_TRUE(my::holds_alternative<std::string>(v1));
    EXPECT_EQ(my::get<std::string>(v1), "hello");
    
    EXPECT_EQ(v2.index(), 0);
    EXPECT_TRUE(my::holds_alternative<int>(v2));
    EXPECT_EQ(my::get<int>(v2), 42);
}

// 测试valueless_by_exception
TEST(VariantTest, ValuelessByException) {
    my::variant<int, std::string> v(42);
    
    EXPECT_FALSE(v.valueless_by_exception());
}

// 测试复杂类型
TEST(VariantTest, ComplexTypes) {
    struct ComplexType {
        int a;
        std::string b;
        std::vector<int> c;
        
        ComplexType(int a, const std::string& b, const std::vector<int>& c)
            : a(a), b(b), c(c) {}
        
        bool operator==(const ComplexType& other) const {
            return a == other.a && b == other.b && c == other.c;
        }
    };
    
    my::variant<int, ComplexType> v(ComplexType(42, "test", {1, 2, 3}));
    EXPECT_EQ(v.index(), 1);
    EXPECT_TRUE(my::holds_alternative<ComplexType>(v));
    EXPECT_EQ(my::get<ComplexType>(v).a, 42);
    EXPECT_EQ(my::get<ComplexType>(v).b, "test");
    EXPECT_EQ(my::get<ComplexType>(v).c, std::vector<int>({1, 2, 3}));
}

// 测试移动语义
TEST(VariantTest, MoveSemantics) {
    my::variant<std::unique_ptr<int>, std::string> v1(std::make_unique<int>(42));
    
    // 移动后，原对象可能处于有效但未指定的状态
    auto moved_v1 = std::move(v1);
    EXPECT_EQ(*my::get<std::unique_ptr<int>>(moved_v1), 42);
}

// 测试类型特征
TEST(VariantTest, TypeTraits) {
    my::variant<int, std::string> v;
    
    EXPECT_TRUE(std::is_copy_constructible_v<decltype(v)>);
    EXPECT_TRUE(std::is_copy_assignable_v<decltype(v)>);
    EXPECT_TRUE(std::is_move_constructible_v<decltype(v)>);
    EXPECT_TRUE(std::is_move_assignable_v<decltype(v)>);
    EXPECT_TRUE(std::is_destructible_v<decltype(v)>);
}

// 测试比较操作
TEST(VariantTest, Comparison) {
    my::variant<int, std::string> v1(42);
    my::variant<int, std::string> v2(42);
    my::variant<int, std::string> v3(100);
    my::variant<int, std::string> v4(std::string("hello"));
    
    EXPECT_TRUE(v1 == v2);
    EXPECT_FALSE(v1 == v3);
    EXPECT_FALSE(v1 == v4);
    
    EXPECT_FALSE(v1 != v2);
    EXPECT_TRUE(v1 != v3);
    EXPECT_TRUE(v1 != v4);
}

// 测试单类型variant
TEST(VariantTest, SingleTypeVariant) {
    my::variant<int> v(42);
    
    EXPECT_EQ(v.index(), 0);
    EXPECT_TRUE(my::holds_alternative<int>(v));
    EXPECT_EQ(my::get<int>(v), 42);
    
    v = 100;
    EXPECT_EQ(my::get<int>(v), 100);
}

// 测试嵌套variant
TEST(VariantTest, NestedVariant) {
    using InnerVariant = my::variant<std::string, double>;
    my::variant<int, InnerVariant> v1(42);
    my::variant<int, InnerVariant> v2(InnerVariant(std::string("hello")));
    
    EXPECT_EQ(v1.index(), 0);
    EXPECT_TRUE(my::holds_alternative<int>(v1));
    EXPECT_EQ(my::get<int>(v1), 42);
    
    EXPECT_EQ(v2.index(), 1);
    EXPECT_TRUE(my::holds_alternative<InnerVariant>(v2));
    auto nested_variant = my::get<InnerVariant>(v2);
    EXPECT_EQ(nested_variant.index(), 0);
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
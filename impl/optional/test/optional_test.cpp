#include <gtest/gtest.h>
#include <string>
#include <vector>
#include <memory>
#include <type_traits>

#include "../include/optional.hpp"

// 测试基本构造和访问
TEST(OptionalTest, BasicConstructionAndAccess) {
    my::optional<int> opt;
    
    // 测试空构造
    EXPECT_FALSE(opt);
    EXPECT_FALSE(opt.has_value());
    
    // 测试值构造
    my::optional<int> opt2(42);
    EXPECT_TRUE(opt2);
    EXPECT_TRUE(opt2.has_value());
    EXPECT_EQ(opt2.value(), 42);
    EXPECT_EQ(*opt2, 42);
    
    // 测试拷贝构造
    my::optional<int> opt3(opt2);
    EXPECT_TRUE(opt3);
    EXPECT_EQ(opt3.value(), 42);
    
    // 测试移动构造
    my::optional<int> opt4(std::move(opt2));
    EXPECT_TRUE(opt4);
    EXPECT_EQ(opt4.value(), 42);
    EXPECT_TRUE(opt2); // 原对象仍然有效（int是基本类型）
}

// 测试赋值操作
TEST(OptionalTest, AssignmentOperations) {
    my::optional<int> opt1;
    my::optional<int> opt2(42);
    
    // 测试值赋值
    opt1 = 100;
    EXPECT_TRUE(opt1);
    EXPECT_EQ(opt1.value(), 100);
    
    // 测试拷贝赋值
    my::optional<int> opt3;
    opt3 = opt1;
    EXPECT_TRUE(opt3);
    EXPECT_EQ(opt3.value(), 100);
    
    // 测试移动赋值
    my::optional<int> opt4;
    opt4 = std::move(opt3);
    EXPECT_TRUE(opt4);
    EXPECT_EQ(opt4.value(), 100);
    EXPECT_TRUE(opt3); // 原对象仍然有效
    
    // 测试重置为空
    opt1.reset();
    EXPECT_FALSE(opt1);
}

// 测试不同类型
TEST(OptionalTest, DifferentTypes) {
    // 字符串类型
    my::optional<std::string> opt_str("hello");
    EXPECT_TRUE(opt_str);
    EXPECT_EQ(opt_str.value(), "hello");
    
    // 复杂类型
    my::optional<std::vector<int>> opt_vec({1, 2, 3});
    EXPECT_TRUE(opt_vec);
    EXPECT_EQ(opt_vec.value().size(), 3);
    EXPECT_EQ(opt_vec.value()[0], 1);
    
    // 指针类型
    auto ptr = std::make_unique<int>(42);
    my::optional<std::unique_ptr<int>> opt_ptr(std::move(ptr));
    EXPECT_TRUE(opt_ptr);
    EXPECT_EQ(*opt_ptr.value(), 42);
}

// 测试value_or方法
TEST(OptionalTest, ValueOrMethod) {
    my::optional<int> opt1(42);
    my::optional<int> opt2;
    
    EXPECT_EQ(opt1.value_or(0), 42);
    EXPECT_EQ(opt2.value_or(0), 0);
    
    EXPECT_EQ(opt1.value_or(100), 42);
    EXPECT_EQ(opt2.value_or(100), 100);
}

// 测试访问操作符
TEST(OptionalTest, AccessOperators) {
    my::optional<int> opt(42);
    my::optional<int> empty_opt;
    
    // 测试operator*
    EXPECT_EQ(*opt, 42);
    
    // 测试operator->
    my::optional<std::string> str_opt("hello");
    EXPECT_EQ(str_opt->size(), 5);
    
    // 测试value方法
    EXPECT_EQ(opt.value(), 42);
    EXPECT_THROW(empty_opt.value(), std::bad_optional_access);
}

// 测试emplace操作
TEST(OptionalTest, EmplaceOperation) {
    my::optional<std::string> opt;
    
    // 测试emplace
    opt.emplace("hello world");
    EXPECT_TRUE(opt);
    EXPECT_EQ(opt.value(), "hello world");
    
    // 测试重新emplace
    opt.emplace("new string");
    EXPECT_TRUE(opt);
    EXPECT_EQ(opt.value(), "new string");
}

// 测试reset操作
TEST(OptionalTest, ResetOperation) {
    my::optional<int> opt(42);
    EXPECT_TRUE(opt);
    
    opt.reset();
    EXPECT_FALSE(opt);
    
    // 重置后可以重新赋值
    opt = 100;
    EXPECT_TRUE(opt);
    EXPECT_EQ(opt.value(), 100);
}

// 测试交换操作
TEST(OptionalTest, SwapOperation) {
    my::optional<int> opt1(42);
    my::optional<int> opt2(100);
    
    opt1.swap(opt2);
    
    EXPECT_EQ(opt1.value(), 100);
    EXPECT_EQ(opt2.value(), 42);
}

// 测试比较操作
TEST(OptionalTest, ComparisonOperations) {
    my::optional<int> opt1(42);
    my::optional<int> opt2(42);
    my::optional<int> opt3(100);
    my::optional<int> opt4;
    
    // 相等比较
    EXPECT_TRUE(opt1 == opt2);
    EXPECT_FALSE(opt1 == opt3);
    EXPECT_FALSE(opt1 == opt4);
    
    // 不等比较
    EXPECT_FALSE(opt1 != opt2);
    EXPECT_TRUE(opt1 != opt3);
    EXPECT_TRUE(opt1 != opt4);
    
    // 小于比较
    EXPECT_FALSE(opt1 < opt2);
    EXPECT_TRUE(opt1 < opt3);
    EXPECT_FALSE(opt3 < opt1);
    EXPECT_TRUE(opt4 < opt1);
    EXPECT_FALSE(opt1 < opt4);
}

// 测试与值的比较
TEST(OptionalTest, ComparisonWithValue) {
    my::optional<int> opt(42);
    my::optional<int> empty_opt;
    
    // 与值比较
    EXPECT_TRUE(opt == 42);
    EXPECT_FALSE(opt == 100);
    EXPECT_FALSE(empty_opt == 42);
    
    EXPECT_FALSE(opt != 42);
    EXPECT_TRUE(opt != 100);
    EXPECT_TRUE(empty_opt != 42);
    
    EXPECT_FALSE(opt < 42);
    EXPECT_TRUE(opt < 100);
    EXPECT_FALSE(42 < opt);
    EXPECT_TRUE(100 > opt);
}

// 测试异常安全
TEST(OptionalTest, ExceptionSafety) {
    // 测试bad_optional_access异常
    my::optional<int> empty_opt;
    EXPECT_THROW(empty_opt.value(), std::bad_optional_access);
    
    // 测试const版本的value方法
    const my::optional<int> const_empty_opt;
    EXPECT_THROW(const_empty_opt.value(), std::bad_optional_access);
}

// 测试类型特征
TEST(OptionalTest, TypeTraits) {
    my::optional<int> opt;
    
    EXPECT_TRUE(std::is_copy_constructible_v<decltype(opt)>);
    EXPECT_TRUE(std::is_copy_assignable_v<decltype(opt)>);
    EXPECT_TRUE(std::is_move_constructible_v<decltype(opt)>);
    EXPECT_TRUE(std::is_move_assignable_v<decltype(opt)>);
    EXPECT_TRUE(std::is_destructible_v<decltype(opt)>);
}

// 测试构造函数异常安全
TEST(OptionalTest, ConstructorExceptionSafety) {
    struct ThrowOnConstruct {
        ThrowOnConstruct() { throw std::runtime_error("Construction failed"); }
    };
    
    EXPECT_THROW(my::optional<ThrowOnConstruct> opt{ThrowOnConstruct()}, std::runtime_error);
}

// 测试赋值异常安全
TEST(OptionalTest, AssignmentExceptionSafety) {
    struct ThrowOnCopy {
        ThrowOnCopy() = default;
        ThrowOnCopy(const ThrowOnCopy&) = default;
        ThrowOnCopy& operator=(const ThrowOnCopy&) { throw std::runtime_error("Copy failed"); }
    };
    
    my::optional<ThrowOnCopy> opt1(ThrowOnCopy{});
    my::optional<ThrowOnCopy> opt2(ThrowOnCopy{});
    
    EXPECT_THROW(opt1 = opt2, std::runtime_error);
}

// 测试emplace异常安全
TEST(OptionalTest, EmplaceExceptionSafety) {
    struct ThrowOnConstruct {
        ThrowOnConstruct(int) { throw std::runtime_error("Construction failed"); }
    };
    
    my::optional<ThrowOnConstruct> opt;
    EXPECT_THROW(opt.emplace(42), std::runtime_error);
}

// 测试移动语义
TEST(OptionalTest, MoveSemantics) {
    my::optional<std::unique_ptr<int>> opt1(std::make_unique<int>(42));
    my::optional<std::unique_ptr<int>> opt2 = std::move(opt1);
    
    EXPECT_TRUE(opt2);
    EXPECT_EQ(*opt2.value(), 42);
    EXPECT_TRUE(opt1); // 移动后原对象仍然有效但值为空
    
    // 移动赋值
    my::optional<std::unique_ptr<int>> opt3;
    opt3 = std::move(opt2);
    EXPECT_TRUE(opt3);
    EXPECT_EQ(*opt3.value(), 42);
    EXPECT_TRUE(opt2); // 移动后原对象仍然有效但值为空
}

// 测试make_optional辅助函数
TEST(OptionalTest, MakeOptionalHelper) {
    auto opt1 = my::make_optional<int>(42);
    EXPECT_TRUE(opt1);
    EXPECT_EQ(opt1.value(), 42);
    
    auto opt2 = my::make_optional<std::string>("hello");
    EXPECT_TRUE(opt2);
    EXPECT_EQ(opt2.value(), "hello");
    
    // 多参数构造
    auto opt3 = my::make_optional<std::vector<int>>(3, 1);
    EXPECT_TRUE(opt3);
    EXPECT_EQ(opt3.value().size(), 3);
    EXPECT_EQ(opt3.value()[0], 1);
}

// 测试nullopt
TEST(OptionalTest, Nullopt) {
    my::optional<int> opt1 = my::nullopt;
    EXPECT_FALSE(opt1);
    
    my::optional<int> opt2(42);
    opt2 = my::nullopt;
    EXPECT_FALSE(opt2);
    
    // 与nullopt比较
    EXPECT_TRUE(opt1 == my::nullopt);
    EXPECT_TRUE(opt2 == my::nullopt);
}

// 测试嵌套optional
TEST(OptionalTest, NestedOptional) {
    my::optional<my::optional<int>> opt1;
    EXPECT_FALSE(opt1);
    
    my::optional<my::optional<int>> opt2(my::optional<int>{42});
    EXPECT_TRUE(opt2);
    EXPECT_TRUE(opt2.value());
    EXPECT_EQ(opt2.value().value(), 42);
    
    my::optional<my::optional<int>> opt3{my::optional<int>{}};
    EXPECT_TRUE(opt3);
    EXPECT_FALSE(opt3.value());
}

// 测试生命周期管理
TEST(OptionalTest, LifetimeManagement) {
    std::shared_ptr<int> shared_counter = std::make_shared<int>(0);
    
    {
        my::optional<std::shared_ptr<int>> opt(shared_counter);
        EXPECT_TRUE(opt);
        EXPECT_EQ(opt.value().use_count(), 2);
        EXPECT_EQ(*opt.value(), 0);
    }
    
    // opt离开作用域，shared_counter引用计数应该减少
    EXPECT_EQ(shared_counter.use_count(), 1);
}

// 测试复杂对象的构造
TEST(OptionalTest, ComplexObjectConstruction) {
    struct ComplexObject {
        int a;
        std::string b;
        std::vector<int> c;
        
        ComplexObject(int a, const std::string& b, const std::vector<int>& c)
            : a(a), b(b), c(c) {}
        
        bool operator==(const ComplexObject& other) const {
            return a == other.a && b == other.b && c == other.c;
        }
    };
    
    // 直接构造
    my::optional<ComplexObject> opt1{ComplexObject(42, "test", {1, 2, 3})};
    EXPECT_TRUE(opt1);
    EXPECT_EQ(opt1.value().a, 42);
    EXPECT_EQ(opt1.value().b, "test");
    EXPECT_EQ(opt1.value().c, std::vector<int>({1, 2, 3}));
    
    // emplace构造
    my::optional<ComplexObject> opt2;
    opt2.emplace(42, std::string("test"), std::vector<int>({1, 2, 3}));
    EXPECT_TRUE(opt2);
    EXPECT_EQ(opt2.value().a, 42);
    EXPECT_EQ(opt2.value().b, "test");
    EXPECT_EQ(opt2.value().c, std::vector<int>({1, 2, 3}));
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
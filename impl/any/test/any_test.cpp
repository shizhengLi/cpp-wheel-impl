#include <gtest/gtest.h>
#include <string>
#include <vector>
#include <memory>
#include <type_traits>

#include "../include/any.hpp"

// 测试基本构造和访问
TEST(AnyTest, BasicConstructionAndAccess) {
    my::any a1 = 42;
    
    EXPECT_TRUE(a1.has_value());
    EXPECT_EQ(a1.type(), typeid(int));
    EXPECT_EQ(my::any_cast<int>(a1), 42);
    
    // 测试字符串类型
    my::any a2 = std::string("hello");
    EXPECT_TRUE(a2.has_value());
    EXPECT_EQ(a2.type(), typeid(std::string));
    EXPECT_EQ(my::any_cast<std::string>(a2), "hello");
    
    // 测试double类型
    my::any a3 = 3.14;
    EXPECT_TRUE(a3.has_value());
    EXPECT_EQ(a3.type(), typeid(double));
    EXPECT_DOUBLE_EQ(my::any_cast<double>(a3), 3.14);
}

// 测试默认构造
TEST(AnyTest, DefaultConstruction) {
    my::any a;
    
    EXPECT_FALSE(a.has_value());
    EXPECT_EQ(a.type(), typeid(void));
}

// 测试拷贝构造
TEST(AnyTest, CopyConstruction) {
    my::any a1 = 42;
    my::any a2 = a1;
    
    EXPECT_TRUE(a2.has_value());
    EXPECT_EQ(a2.type(), typeid(int));
    EXPECT_EQ(my::any_cast<int>(a2), 42);
    
    // 原对象不受影响
    EXPECT_EQ(my::any_cast<int>(a1), 42);
}

// 测试移动构造
TEST(AnyTest, MoveConstruction) {
    my::any a1 = std::string("hello");
    my::any a2 = std::move(a1);
    
    EXPECT_TRUE(a2.has_value());
    EXPECT_EQ(a2.type(), typeid(std::string));
    EXPECT_EQ(my::any_cast<std::string>(a2), "hello");
    
    // 原对象可能为空
    EXPECT_FALSE(a1.has_value());
}

// 测试拷贝赋值
TEST(AnyTest, CopyAssignment) {
    my::any a1 = 42;
    my::any a2;
    
    a2 = a1;
    EXPECT_TRUE(a2.has_value());
    EXPECT_EQ(a2.type(), typeid(int));
    EXPECT_EQ(my::any_cast<int>(a2), 42);
    
    // 原对象不受影响
    EXPECT_EQ(my::any_cast<int>(a1), 42);
}

// 测试移动赋值
TEST(AnyTest, MoveAssignment) {
    my::any a1 = std::string("hello");
    my::any a2;
    
    a2 = std::move(a1);
    EXPECT_TRUE(a2.has_value());
    EXPECT_EQ(a2.type(), typeid(std::string));
    EXPECT_EQ(my::any_cast<std::string>(a2), "hello");
    
    // 原对象可能为空
    EXPECT_FALSE(a1.has_value());
}

// 测试值赋值
TEST(AnyTest, ValueAssignment) {
    my::any a;
    
    a = 42;
    EXPECT_TRUE(a.has_value());
    EXPECT_EQ(a.type(), typeid(int));
    EXPECT_EQ(my::any_cast<int>(a), 42);
    
    a = std::string("hello");
    EXPECT_TRUE(a.has_value());
    EXPECT_EQ(a.type(), typeid(std::string));
    EXPECT_EQ(my::any_cast<std::string>(a), "hello");
}

// 测试reset操作
TEST(AnyTest, ResetOperation) {
    my::any a = 42;
    EXPECT_TRUE(a.has_value());
    
    a.reset();
    EXPECT_FALSE(a.has_value());
    EXPECT_EQ(a.type(), typeid(void));
}

// 测试swap操作
TEST(AnyTest, SwapOperation) {
    my::any a1 = 42;
    my::any a2 = std::string("hello");
    
    a1.swap(a2);
    
    EXPECT_TRUE(a1.has_value());
    EXPECT_EQ(a1.type(), typeid(std::string));
    EXPECT_EQ(my::any_cast<std::string>(a1), "hello");
    
    EXPECT_TRUE(a2.has_value());
    EXPECT_EQ(a2.type(), typeid(int));
    EXPECT_EQ(my::any_cast<int>(a2), 42);
}

// 测试any_cast异常
TEST(AnyTest, AnyCastException) {
    my::any a = 42;
    
    EXPECT_THROW(my::any_cast<std::string>(a), my::bad_any_cast);
    EXPECT_THROW(my::any_cast<double>(a), my::bad_any_cast);
}

// 测试指针形式的any_cast
TEST(AnyTest, PointerAnyCast) {
    my::any a = 42;
    
    // 正确类型
    auto ptr = my::any_cast<int>(&a);
    ASSERT_NE(ptr, nullptr);
    EXPECT_EQ(*ptr, 42);
    
    // 错误类型
    auto wrong_ptr = my::any_cast<std::string>(&a);
    EXPECT_EQ(wrong_ptr, nullptr);
    
    // 空any
    my::any empty;
    auto empty_ptr = my::any_cast<int>(&empty);
    EXPECT_EQ(empty_ptr, nullptr);
}

// 测试const any_cast
TEST(AnyTest, ConstAnyCast) {
    const my::any a = 42;
    
    EXPECT_EQ(my::any_cast<int>(a), 42);
    EXPECT_THROW(my::any_cast<std::string>(a), my::bad_any_cast);
}

// 测试移动语义的any_cast
TEST(AnyTest, MoveAnyCast) {
    my::any a = std::string("hello");
    
    // 移动cast
    std::string result = my::any_cast<std::string>(std::move(a));
    EXPECT_EQ(result, "hello");
    
    // 原对象可能处于有效但未指定的状态
}

// 测试复杂类型
TEST(AnyTest, ComplexTypes) {
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
    
    my::any a(ComplexType(42, "test", {1, 2, 3}));
    EXPECT_TRUE(a.has_value());
    EXPECT_EQ(a.type(), typeid(ComplexType));
    
    auto value = my::any_cast<ComplexType>(a);
    EXPECT_EQ(value.a, 42);
    EXPECT_EQ(value.b, "test");
    EXPECT_EQ(value.c, std::vector<int>({1, 2, 3}));
}

// 测试unique_ptr (简化版，不测试拷贝)
TEST(AnyTest, UniquePtr) {
    // 注意：unique_ptr不能拷贝，所以不能放入any中
    // 这里我们只测试可以移动构造的类型
    struct MovableOnly {
        int value;
        MovableOnly(int v) : value(v) {}
        MovableOnly(MovableOnly&&) = default;
        MovableOnly& operator=(MovableOnly&&) = default;
        MovableOnly(const MovableOnly&) = delete;
        MovableOnly& operator=(const MovableOnly&) = delete;
    };
    
    MovableOnly obj(42);
    my::any a(std::move(obj));
    
    EXPECT_TRUE(a.has_value());
    EXPECT_EQ(a.type(), typeid(MovableOnly));
    
    auto result = my::any_cast<MovableOnly>(std::move(a));
    EXPECT_EQ(result.value, 42);
}

// 测试make_any辅助函数
TEST(AnyTest, MakeAny) {
    auto a = my::make_any<int>(42);
    EXPECT_TRUE(a.has_value());
    EXPECT_EQ(a.type(), typeid(int));
    EXPECT_EQ(my::any_cast<int>(a), 42);
    
    auto b = my::make_any<std::string>("hello");
    EXPECT_TRUE(b.has_value());
    EXPECT_EQ(b.type(), typeid(std::string));
    EXPECT_EQ(my::any_cast<std::string>(b), "hello");
    
    // 多参数构造
    auto c = my::make_any<std::vector<int>>(3, 1);
    EXPECT_TRUE(c.has_value());
    EXPECT_EQ(c.type(), typeid(std::vector<int>));
    auto vec = my::any_cast<std::vector<int>>(c);
    EXPECT_EQ(vec.size(), 3);
    EXPECT_EQ(vec[0], 1);
}

// 测试类型特征
TEST(AnyTest, TypeTraits) {
    my::any a;
    
    EXPECT_TRUE(std::is_copy_constructible_v<decltype(a)>);
    EXPECT_TRUE(std::is_copy_assignable_v<decltype(a)>);
    EXPECT_TRUE(std::is_move_constructible_v<decltype(a)>);
    EXPECT_TRUE(std::is_move_assignable_v<decltype(a)>);
    EXPECT_TRUE(std::is_destructible_v<decltype(a)>);
}

// 测试自赋值
TEST(AnyTest, SelfAssignment) {
    my::any a = 42;
    a = a;
    EXPECT_TRUE(a.has_value());
    EXPECT_EQ(my::any_cast<int>(a), 42);
}

// 测试链式赋值
TEST(AnyTest, ChainedAssignment) {
    my::any a, b, c;
    a = b = c = 42;
    
    EXPECT_TRUE(a.has_value());
    EXPECT_TRUE(b.has_value());
    EXPECT_TRUE(c.has_value());
    EXPECT_EQ(my::any_cast<int>(a), 42);
    EXPECT_EQ(my::any_cast<int>(b), 42);
    EXPECT_EQ(my::any_cast<int>(c), 42);
}

// 测试const类型
TEST(AnyTest, ConstTypes) {
    const int value = 42;
    my::any a = value;
    
    EXPECT_TRUE(a.has_value());
    EXPECT_EQ(a.type(), typeid(int));
    EXPECT_EQ(my::any_cast<int>(a), 42);
}

// 测试引用类型
TEST(AnyTest, ReferenceTypes) {
    int value = 42;
    int& ref = value;
    my::any a = ref;
    
    EXPECT_TRUE(a.has_value());
    EXPECT_EQ(a.type(), typeid(int));
    EXPECT_EQ(my::any_cast<int>(a), 42);
}

// 测试异常安全
TEST(AnyTest, ExceptionSafety) {
    // 测试bad_any_cast异常
    my::any a = 42;
    EXPECT_THROW(my::any_cast<std::string>(a), my::bad_any_cast);
    
    // 测试const版本的any_cast
    const my::any const_a = 42;
    EXPECT_THROW(my::any_cast<std::string>(const_a), my::bad_any_cast);
}

// 测试空any的操作
TEST(AnyTest, EmptyAnyOperations) {
    my::any a;
    
    // 空any的swap
    my::any b;
    a.swap(b);
    EXPECT_FALSE(a.has_value());
    EXPECT_FALSE(b.has_value());
    
    // 空any的reset
    a.reset();
    EXPECT_FALSE(a.has_value());
    
    // 空any的类型
    EXPECT_EQ(a.type(), typeid(void));
}

// 测试生命周期管理
TEST(AnyTest, LifetimeManagement) {
    std::shared_ptr<int> shared_counter = std::make_shared<int>(0);
    auto initial_count = shared_counter.use_count();
    
    {
        my::any a = shared_counter;
        EXPECT_TRUE(a.has_value());
        // 使用指针形式避免创建临时对象
        auto ptr = my::any_cast<std::shared_ptr<int>>(&a);
        EXPECT_EQ(ptr->use_count(), initial_count + 1);
        EXPECT_EQ(**ptr, 0);
    }
    
    // a离开作用域，shared_counter引用计数应该减少
    EXPECT_EQ(shared_counter.use_count(), initial_count);
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
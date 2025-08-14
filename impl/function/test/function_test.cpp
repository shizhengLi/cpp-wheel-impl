#include <gtest/gtest.h>
#include <memory>
#include <functional>
#include <type_traits>

#include "../include/function.hpp"

// 测试基本构造和调用
TEST(FunctionTest, BasicConstructionAndCall) {
    my::function<int(int, int)> add;
    
    // 测试空构造
    EXPECT_FALSE(add);
    
    // 测试lambda赋值
    add = [](int a, int b) { return a + b; };
    EXPECT_TRUE(add);
    EXPECT_EQ(add(2, 3), 5);
    
    // 测试构造函数
    my::function<int(int, int)> multiply = [](int a, int b) { return a * b; };
    EXPECT_TRUE(multiply);
    EXPECT_EQ(multiply(3, 4), 12);
}

// 测试拷贝构造和赋值
TEST(FunctionTest, CopyConstructionAndAssignment) {
    my::function<int(int)> square = [](int x) { return x * x; };
    
    // 拷贝构造
    my::function<int(int)> square_copy(square);
    EXPECT_TRUE(square_copy);
    EXPECT_EQ(square_copy(5), 25);
    EXPECT_EQ(square(5), 25); // 原函数仍然可用
    
    // 拷贝赋值
    my::function<int(int)> square_assign;
    square_assign = square;
    EXPECT_TRUE(square_assign);
    EXPECT_EQ(square_assign(6), 36);
    EXPECT_EQ(square(6), 36); // 原函数仍然可用
}

// 测试移动构造和赋值
TEST(FunctionTest, MoveConstructionAndAssignment) {
    my::function<std::string(int)> to_string = [](int x) {
        return std::to_string(x);
    };
    
    // 移动构造
    my::function<std::string(int)> to_string_move(std::move(to_string));
    EXPECT_TRUE(to_string_move);
    EXPECT_EQ(to_string_move(42), "42");
    EXPECT_FALSE(to_string); // 原函数应为空
    
    // 重新赋值原函数
    to_string = [](int x) { return std::to_string(x * 2); };
    
    // 移动赋值
    my::function<std::string(int)> to_string_assign;
    to_string_assign = std::move(to_string);
    EXPECT_TRUE(to_string_assign);
    EXPECT_EQ(to_string_assign(21), "42");
    EXPECT_FALSE(to_string); // 原函数应为空
}

// 测试不同类型的可调用对象
TEST(FunctionTest, DifferentCallableTypes) {
    // 普通函数
    int (*func_ptr)(int) = [](int x) { return x + 1; };
    my::function<int(int)> f1 = func_ptr;
    EXPECT_EQ(f1(10), 11);
    
    // 函数对象
    struct Adder {
        int operator()(int x, int y) const { return x + y; }
    };
    Adder adder;
    my::function<int(int, int)> f2 = adder;
    EXPECT_EQ(f2(5, 7), 12);
    
    // 带状态的lambda
    int counter = 0;
    my::function<int()> f3 = [counter]() mutable { return ++counter; };
    EXPECT_EQ(f3(), 1);
    EXPECT_EQ(f3(), 2);
    
    // 成员函数
    struct Calculator {
        int add(int x, int y) const { return x + y; }
    };
    Calculator calc;
    my::function<int(int, int)> f4 = [&calc](int x, int y) { return calc.add(x, y); };
    EXPECT_EQ(f4(8, 9), 17);
}

// 测试空函数调用异常
TEST(FunctionTest, EmptyFunctionCall) {
    my::function<int()> empty_func;
    EXPECT_FALSE(empty_func);
    
    EXPECT_THROW(empty_func(), std::bad_function_call);
}

// 测试重置操作
TEST(FunctionTest, ResetOperations) {
    my::function<int(int)> func = [](int x) { return x * 2; };
    EXPECT_TRUE(func);
    EXPECT_EQ(func(5), 10);
    
    // 重置为nullptr
    func = nullptr;
    EXPECT_FALSE(func);
    
    // 重新赋值
    func = [](int x) { return x * 3; };
    EXPECT_TRUE(func);
    EXPECT_EQ(func(5), 15);
}

// 测试交换操作
TEST(FunctionTest, SwapOperation) {
    my::function<int(int)> f1 = [](int x) { return x + 1; };
    my::function<int(int)> f2 = [](int x) { return x * 2; };
    
    EXPECT_EQ(f1(5), 6);
    EXPECT_EQ(f2(5), 10);
    
    f1.swap(f2);
    
    EXPECT_EQ(f1(5), 10);
    EXPECT_EQ(f2(5), 6);
}

// 测试不同返回类型
TEST(FunctionTest, DifferentReturnTypes) {
    // void返回类型
    my::function<void(int)> print = [](int x) {
        std::cout << x << std::endl;
    };
    EXPECT_TRUE(print);
    print(42); // 不抛出异常
    
    // bool返回类型
    my::function<bool(int)> is_even = [](int x) { return x % 2 == 0; };
    EXPECT_TRUE(is_even);
    EXPECT_TRUE(is_even(4));
    EXPECT_FALSE(is_even(5));
    
    // 复杂类型返回
    my::function<std::vector<int>(int)> create_vector = [](int size) {
        return std::vector<int>(size, 1);
    };
    EXPECT_TRUE(create_vector);
    auto vec = create_vector(3);
    EXPECT_EQ(vec.size(), 3);
    EXPECT_EQ(vec[0], 1);
    EXPECT_EQ(vec[1], 1);
    EXPECT_EQ(vec[2], 1);
}

// 测试多参数函数
TEST(FunctionTest, MultipleParameters) {
    // 无参数
    my::function<int()> zero_args = []() { return 42; };
    EXPECT_EQ(zero_args(), 42);
    
    // 一个参数
    my::function<int(int)> one_arg = [](int x) { return x * 2; };
    EXPECT_EQ(one_arg(21), 42);
    
    // 两个参数
    my::function<int(int, int)> two_args = [](int x, int y) { return x + y; };
    EXPECT_EQ(two_args(20, 22), 42);
    
    // 三个参数
    my::function<int(int, int, int)> three_args = [](int x, int y, int z) { return x + y + z; };
    EXPECT_EQ(three_args(10, 20, 12), 42);
}

// 测试引用参数
TEST(FunctionTest, ReferenceParameters) {
    my::function<void(int&)> increment = [](int& x) { x++; };
    
    int value = 10;
    increment(value);
    EXPECT_EQ(value, 11);
    
    my::function<int&(int&)> get_ref = [](int& x) -> int& { return x; };
    
    int num = 20;
    int& ref = get_ref(num);
    ref = 30;
    EXPECT_EQ(num, 30);
}

// 测试const函数
TEST(FunctionTest, ConstFunctions) {
    my::function<int(int)> const_func = [](int x) { return x * 2; };
    
    const auto& const_ref = const_func;
    EXPECT_EQ(const_ref(21), 42);
}

// 测试类型特征
TEST(FunctionTest, TypeTraits) {
    my::function<int(int)> func;
    
    EXPECT_TRUE(std::is_copy_constructible_v<decltype(func)>);
    EXPECT_TRUE(std::is_copy_assignable_v<decltype(func)>);
    EXPECT_TRUE(std::is_move_constructible_v<decltype(func)>);
    EXPECT_TRUE(std::is_move_assignable_v<decltype(func)>);
}

// 测试异常安全
TEST(FunctionTest, ExceptionSafety) {
    my::function<int(int)> throwing_func = [](int x) {
        if (x < 0) {
            throw std::runtime_error("Negative value");
        }
        return x * 2;
    };
    
    EXPECT_EQ(throwing_func(5), 10);
    EXPECT_THROW(throwing_func(-1), std::runtime_error);
}

// 测试函数指针兼容性
TEST(FunctionTest, FunctionPointerCompatibility) {
    // 普通函数
    auto regular_func = [](int x) {
        return x * 3;
    };
    
    my::function<int(int)> f1 = regular_func;
    EXPECT_EQ(f1(7), 21);
    
    // 函数指针
    auto func_ptr = +regular_func; // 转换为函数指针
    my::function<int(int)> f2 = func_ptr;
    EXPECT_EQ(f2(8), 24);
}

// 测试std::function兼容性
TEST(FunctionTest, StdFunctionCompatibility) {
    std::function<int(int)> std_func = [](int x) { return x + 10; };
    
    // 从std::function构造
    my::function<int(int)> my_func = std_func;
    EXPECT_EQ(my_func(5), 15);
    
    // 赋值std::function
    my_func = std_func;
    EXPECT_EQ(my_func(6), 16);
}

// 测试make_function辅助函数
TEST(FunctionTest, MakeFunctionHelper) {
    auto lambda = [](int x, int y) { return x * y; };
    
    auto func = my::make_function<int, int, int>(lambda);
    EXPECT_TRUE(func);
    EXPECT_EQ(func(6, 7), 42);
}

// 测试目标类型查询
TEST(FunctionTest, TargetTypeQuery) {
    my::function<int(int)> func = [](int x) { return x * 2; };
    
    EXPECT_TRUE(func);
    EXPECT_NE(func.target_type(), typeid(void));
    
    func = nullptr;
    EXPECT_FALSE(func);
    EXPECT_EQ(func.target_type(), typeid(void));
}

// 测试生命周期管理
TEST(FunctionTest, LifetimeManagement) {
    std::shared_ptr<int> shared_counter = std::make_shared<int>(0);
    
    {
        my::function<int()> func = [shared_counter]() {
            return ++(*shared_counter);
        };
        
        EXPECT_EQ(func(), 1);
        EXPECT_EQ(*shared_counter, 1);
        EXPECT_EQ(func(), 2);
        EXPECT_EQ(*shared_counter, 2);
    }
    
    // func离开作用域，shared_counter引用计数应该减少
    EXPECT_EQ(shared_counter.use_count(), 1);
}

// 测试递归函数
TEST(FunctionTest, RecursiveFunction) {
    my::function<int(int)> factorial;
    
    factorial = [&factorial](int n) -> int {
        if (n <= 1) return 1;
        return n * factorial(n - 1);
    };
    
    EXPECT_EQ(factorial(5), 120);
    EXPECT_EQ(factorial(0), 1);
    EXPECT_EQ(factorial(1), 1);
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
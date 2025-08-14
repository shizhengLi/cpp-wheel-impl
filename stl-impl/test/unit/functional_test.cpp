#include <gtest/gtest.h>
#include <vector>
#include <string>
#include <memory>
#include "stl/functional.hpp"

using namespace stl;

class FunctionalTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

// 算术运算符测试
TEST_F(FunctionalTest, ArithmeticOperators) {
    // plus
    plus<int> add;
    EXPECT_EQ(add(2, 3), 5);
    
    plus<> generic_add;
    EXPECT_EQ(generic_add(2.5, 3.5), 6.0);
    
    // minus
    minus<int> sub;
    EXPECT_EQ(sub(5, 3), 2);
    
    minus<> generic_sub;
    EXPECT_EQ(generic_sub(5.5, 3.5), 2.0);
    
    // multiplies
    multiplies<int> mul;
    EXPECT_EQ(mul(3, 4), 12);
    
    multiplies<> generic_mul;
    EXPECT_EQ(generic_mul(3.5, 2.0), 7.0);
    
    // divides
    divides<int> div;
    EXPECT_EQ(div(10, 2), 5);
    
    divides<> generic_div;
    EXPECT_EQ(generic_div(10.0, 2.0), 5.0);
    
    // modulus
    modulus<int> mod;
    EXPECT_EQ(mod(10, 3), 1);
    
    modulus<> generic_mod;
    EXPECT_EQ(generic_mod(10, 3), 1);
    
    // negate
    negate<int> neg;
    EXPECT_EQ(neg(5), -5);
    EXPECT_EQ(neg(-3), 3);
    
    negate<> generic_neg;
    EXPECT_EQ(generic_neg(5.5), -5.5);
}

// 比较运算符测试
TEST_F(FunctionalTest, ComparisonOperators) {
    // equal_to
    equal_to<int> eq;
    EXPECT_TRUE(eq(5, 5));
    EXPECT_FALSE(eq(5, 3));
    
    equal_to<> generic_eq;
    EXPECT_TRUE(generic_eq(5.0, 5.0));
    EXPECT_FALSE(generic_eq(5.0, 3.0));
    
    // not_equal_to
    not_equal_to<int> neq;
    EXPECT_TRUE(neq(5, 3));
    EXPECT_FALSE(neq(5, 5));
    
    not_equal_to<> generic_neq;
    EXPECT_TRUE(generic_neq(5.0, 3.0));
    EXPECT_FALSE(generic_neq(5.0, 5.0));
    
    // greater
    greater<int> gt;
    EXPECT_TRUE(gt(5, 3));
    EXPECT_FALSE(gt(3, 5));
    
    greater<> generic_gt;
    EXPECT_TRUE(generic_gt(5.5, 3.5));
    EXPECT_FALSE(generic_gt(3.5, 5.5));
    
    // less
    less<int> lt;
    EXPECT_TRUE(lt(3, 5));
    EXPECT_FALSE(lt(5, 3));
    
    less<> generic_lt;
    EXPECT_TRUE(generic_lt(3.5, 5.5));
    EXPECT_FALSE(generic_lt(5.5, 3.5));
    
    // greater_equal
    greater_equal<int> ge;
    EXPECT_TRUE(ge(5, 3));
    EXPECT_TRUE(ge(5, 5));
    EXPECT_FALSE(ge(3, 5));
    
    greater_equal<> generic_ge;
    EXPECT_TRUE(generic_ge(5.5, 3.5));
    EXPECT_TRUE(generic_ge(5.5, 5.5));
    EXPECT_FALSE(generic_ge(3.5, 5.5));
    
    // less_equal
    less_equal<int> le;
    EXPECT_TRUE(le(3, 5));
    EXPECT_TRUE(le(5, 5));
    EXPECT_FALSE(le(5, 3));
    
    less_equal<> generic_le;
    EXPECT_TRUE(generic_le(3.5, 5.5));
    EXPECT_TRUE(generic_le(5.5, 5.5));
    EXPECT_FALSE(generic_le(5.5, 3.5));
}

// 逻辑运算符测试
TEST_F(FunctionalTest, LogicalOperators) {
    // logical_and
    logical_and<bool> land;
    EXPECT_TRUE(land(true, true));
    EXPECT_FALSE(land(true, false));
    EXPECT_FALSE(land(false, true));
    EXPECT_FALSE(land(false, false));
    
    logical_and<> generic_land;
    EXPECT_TRUE(generic_land(true, true));
    EXPECT_FALSE(generic_land(true, false));
    
    // logical_or
    logical_or<bool> lor;
    EXPECT_TRUE(lor(true, true));
    EXPECT_TRUE(lor(true, false));
    EXPECT_TRUE(lor(false, true));
    EXPECT_FALSE(lor(false, false));
    
    logical_or<> generic_lor;
    EXPECT_TRUE(generic_lor(true, false));
    EXPECT_FALSE(generic_lor(false, false));
    
    // logical_not
    logical_not<bool> lnot;
    EXPECT_FALSE(lnot(true));
    EXPECT_TRUE(lnot(false));
    
    logical_not<> generic_lnot;
    EXPECT_FALSE(generic_lnot(true));
    EXPECT_TRUE(generic_lnot(false));
}

// 位运算符测试
TEST_F(FunctionalTest, BitwiseOperators) {
    // bit_and
    bit_and<int> band;
    EXPECT_EQ(band(0b1010, 0b1100), 0b1000);
    
    bit_and<> generic_band;
    EXPECT_EQ(generic_band(0b1010, 0b1100), 0b1000);
    
    // bit_or
    bit_or<int> bor;
    EXPECT_EQ(bor(0b1010, 0b1100), 0b1110);
    
    bit_or<> generic_bor;
    EXPECT_EQ(generic_bor(0b1010, 0b1100), 0b1110);
    
    // bit_xor
    bit_xor<int> bxor;
    EXPECT_EQ(bxor(0b1010, 0b1100), 0b0110);
    
    bit_xor<> generic_bxor;
    EXPECT_EQ(generic_bxor(0b1010, 0b1100), 0b0110);
    
    // bit_not
    bit_not<int> bnot;
    EXPECT_EQ(bnot(0b1010), ~0b1010);
    
    bit_not<> generic_bnot;
    EXPECT_EQ(generic_bnot(0b1010), ~0b1010);
}

// 函数对象适配器测试
TEST_F(FunctionalTest, FunctionAdapters) {
    // unary_negate
    auto is_positive = [](int x) { return x > 0; };
    auto not_positive = not1(is_positive);
    EXPECT_FALSE(not_positive(5));
    EXPECT_TRUE(not_positive(-3));
    EXPECT_TRUE(not_positive(0));
    
    // binary_negate
    equal_to<int> eq;
    auto not_equal = not2(eq);
    EXPECT_TRUE(not_equal(5, 3));
    EXPECT_FALSE(not_equal(5, 5));
    
    // binder_front - 简单绑定
    auto add_five = bind_front(plus<int>(), 5);
    EXPECT_EQ(add_five(3), 8);
    EXPECT_EQ(add_five(10), 15);
}

// 引用包装器测试
TEST_F(FunctionalTest, ReferenceWrapper) {
    int x = 42;
    reference_wrapper<int> ref_x = ref(x);
    
    EXPECT_EQ(ref_x.get(), 42);
    EXPECT_EQ(static_cast<int&>(ref_x), 42);
    
    // 修改原值
    ref_x.get() = 100;
    EXPECT_EQ(x, 100);
    
    // const引用
    const int y = 123;
    reference_wrapper<const int> ref_y = cref(y);
    EXPECT_EQ(ref_y.get(), 123);
    
    // 引用包装器的引用包装器
    auto ref_ref = ref(ref_x);
    EXPECT_EQ(ref_ref.get(), 100);
}

// 哈希函数测试
TEST_F(FunctionalTest, HashFunction) {
    // 基本类型哈希
    hash<int> int_hash;
    EXPECT_EQ(int_hash(42), static_cast<size_t>(42));
    
    hash<bool> bool_hash;
    EXPECT_EQ(bool_hash(true), static_cast<size_t>(true));
    EXPECT_EQ(bool_hash(false), static_cast<size_t>(false));
    
    hash<float> float_hash;
    float f = 3.14f;
    size_t result = float_hash(f);
    // 验证哈希函数确实在工作
    EXPECT_NE(result, 0);
    EXPECT_EQ(float_hash(f), float_hash(f)); // 相同输入应该产生相同输出
    
    // 指针哈希
    hash<int*> ptr_hash;
    int x = 42;
    EXPECT_EQ(ptr_hash(&x), reinterpret_cast<size_t>(&x));
    
    // 字符哈希
    hash<char> char_hash;
    EXPECT_EQ(char_hash('A'), static_cast<size_t>('A'));
}

// 恒等函数测试
TEST_F(FunctionalTest, IdentityFunction) {
    identity<int> id;
    int x = 42;
    EXPECT_EQ(id(std::move(x)), 42);
    
    identity<> generic_id;
    EXPECT_EQ(generic_id(3.14), 3.14);
    EXPECT_EQ(generic_id(std::string("hello")), "hello");
    
    // 移动语义
    std::unique_ptr<int> ptr = std::make_unique<int>(42);
    auto moved_ptr = generic_id(std::move(ptr));
    EXPECT_EQ(*moved_ptr, 42);
    EXPECT_EQ(ptr.get(), nullptr);
}

// 类型特征测试
TEST_F(FunctionalTest, TypeTraits) {
    // 检查一元函数类型特征
    static_assert(std::is_same_v<unary_function<int, double>::argument_type, int>);
    static_assert(std::is_same_v<unary_function<int, double>::result_type, double>);
    
    // 检查二元函数类型特征
    static_assert(std::is_same_v<binary_function<int, double, bool>::first_argument_type, int>);
    static_assert(std::is_same_v<binary_function<int, double, bool>::second_argument_type, double>);
    static_assert(std::is_same_v<binary_function<int, double, bool>::result_type, bool>);
    
    // 检查算术运算符的类型特征
    static_assert(std::is_same_v<plus<int>::first_argument_type, int>);
    static_assert(std::is_same_v<plus<int>::second_argument_type, int>);
    static_assert(std::is_same_v<plus<int>::result_type, int>);
    
    // 检查比较运算符的类型特征
    static_assert(std::is_same_v<equal_to<int>::first_argument_type, int>);
    static_assert(std::is_same_v<equal_to<int>::second_argument_type, int>);
    static_assert(std::is_same_v<equal_to<int>::result_type, bool>);
}

// 标准库兼容性测试
TEST_F(FunctionalTest, StandardLibraryCompatibility) {
    // 与STL算法配合使用
    std::vector<int> vec = {1, 2, 3, 4, 5};
    
    // 使用less进行排序
    std::sort(vec.begin(), vec.end(), less<int>());
    EXPECT_EQ(vec, std::vector<int>({1, 2, 3, 4, 5}));
    
    // 使用greater进行排序
    std::sort(vec.begin(), vec.end(), greater<int>());
    EXPECT_EQ(vec, std::vector<int>({5, 4, 3, 2, 1}));
}

// 完美转发测试
TEST_F(FunctionalTest, PerfectForwarding) {
    // 测试void特化的完美转发
    plus<> generic_plus;
    
    // 左值
    int a = 1, b = 2;
    EXPECT_EQ(generic_plus(a, b), 3);
    
    // 右值
    EXPECT_EQ(generic_plus(1, 2), 3);
    EXPECT_EQ(generic_plus(1.5, 2.5), 4.0);
    
    // 混合类型
    EXPECT_EQ(generic_plus(1, 2.5), 3.5);
    
    // 逻辑运算符的完美转发
    logical_and<> generic_and;
    EXPECT_TRUE(generic_and(true, true));
    EXPECT_FALSE(generic_and(true, false));
    
    // 比较运算符的完美转发
    less<> generic_less;
    EXPECT_TRUE(generic_less(1, 2));
    EXPECT_FALSE(generic_less(2, 1));
}

// 常量表达式测试
TEST_F(FunctionalTest, ConstexprSupport) {
    // 算术运算符
    constexpr plus<int> c_add;
    static_assert(c_add(2, 3) == 5);
    
    constexpr minus<int> c_sub;
    static_assert(c_sub(5, 3) == 2);
    
    // 比较运算符
    constexpr equal_to<int> c_eq;
    static_assert(c_eq(5, 5));
    static_assert(!c_eq(5, 3));
    
    constexpr less<int> c_less;
    static_assert(c_less(3, 5));
    static_assert(!c_less(5, 3));
    
    // 逻辑运算符
    constexpr logical_and<bool> c_and;
    static_assert(c_and(true, true));
    static_assert(!c_and(true, false));
    
    constexpr logical_not<bool> c_not;
    static_assert(!c_not(true));
    static_assert(c_not(false));
    
    // 位运算符
    constexpr bit_and<int> c_band;
    static_assert(c_band(0b1010, 0b1100) == 0b1000);
    
    constexpr bit_or<int> c_bor;
    static_assert(c_bor(0b1010, 0b1100) == 0b1110);
}

// 异常安全测试
TEST_F(FunctionalTest, ExceptionSafety) {
    // 引用包装器不应该抛出异常
    int x = 42;
    reference_wrapper<int> ref_x = stl::ref(x);
    EXPECT_NO_THROW(ref_x.get());
    
    // 基本运算符不应该抛出异常
    plus<int> add;
    EXPECT_NO_THROW(add(1, 2));
    
    equal_to<int> eq;
    EXPECT_NO_THROW(eq(1, 2));
    
    // 哈希函数不应该抛出异常
    hash<int> h;
    EXPECT_NO_THROW(h(42));
}
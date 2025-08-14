#pragma once

#include <type_traits>
#include <utility>
#include <functional>
#include <string>
#include <memory>

namespace stl {

// 基础模板定义
template<typename Arg, typename Result>
struct unary_function {
    using argument_type = Arg;
    using result_type = Result;
};

template<typename Arg1, typename Arg2, typename Result>
struct binary_function {
    using first_argument_type = Arg1;
    using second_argument_type = Arg2;
    using result_type = Result;
};

// 算术运算符
template<typename T = void>
struct plus : binary_function<T, T, T> {
    constexpr T operator()(const T& x, const T& y) const {
        return x + y;
    }
};

template<>
struct plus<void> {
    template<typename T, typename U>
    constexpr auto operator()(T&& x, U&& y) const
        -> decltype(std::forward<T>(x) + std::forward<U>(y)) {
        return std::forward<T>(x) + std::forward<U>(y);
    }
};

template<typename T = void>
struct minus : binary_function<T, T, T> {
    constexpr T operator()(const T& x, const T& y) const {
        return x - y;
    }
};

template<>
struct minus<void> {
    template<typename T, typename U>
    constexpr auto operator()(T&& x, U&& y) const
        -> decltype(std::forward<T>(x) - std::forward<U>(y)) {
        return std::forward<T>(x) - std::forward<U>(y);
    }
};

template<typename T = void>
struct multiplies : binary_function<T, T, T> {
    constexpr T operator()(const T& x, const T& y) const {
        return x * y;
    }
};

template<>
struct multiplies<void> {
    template<typename T, typename U>
    constexpr auto operator()(T&& x, U&& y) const
        -> decltype(std::forward<T>(x) * std::forward<U>(y)) {
        return std::forward<T>(x) * std::forward<U>(y);
    }
};

template<typename T = void>
struct divides : binary_function<T, T, T> {
    constexpr T operator()(const T& x, const T& y) const {
        return x / y;
    }
};

template<>
struct divides<void> {
    template<typename T, typename U>
    constexpr auto operator()(T&& x, U&& y) const
        -> decltype(std::forward<T>(x) / std::forward<U>(y)) {
        return std::forward<T>(x) / std::forward<U>(y);
    }
};

template<typename T = void>
struct modulus : binary_function<T, T, T> {
    constexpr T operator()(const T& x, const T& y) const {
        return x % y;
    }
};

template<>
struct modulus<void> {
    template<typename T, typename U>
    constexpr auto operator()(T&& x, U&& y) const
        -> decltype(std::forward<T>(x) % std::forward<U>(y)) {
        return std::forward<T>(x) % std::forward<U>(y);
    }
};

template<typename T = void>
struct negate : unary_function<T, T> {
    constexpr T operator()(const T& x) const {
        return -x;
    }
};

template<>
struct negate<void> {
    template<typename T>
    constexpr auto operator()(T&& x) const
        -> decltype(-std::forward<T>(x)) {
        return -std::forward<T>(x);
    }
};

// 比较运算符
template<typename T = void>
struct equal_to : binary_function<T, T, bool> {
    constexpr bool operator()(const T& x, const T& y) const {
        return x == y;
    }
};

template<>
struct equal_to<void> {
    template<typename T, typename U>
    constexpr auto operator()(T&& x, U&& y) const
        -> decltype(std::forward<T>(x) == std::forward<U>(y)) {
        return std::forward<T>(x) == std::forward<U>(y);
    }
};

template<typename T = void>
struct not_equal_to : binary_function<T, T, bool> {
    constexpr bool operator()(const T& x, const T& y) const {
        return x != y;
    }
};

template<>
struct not_equal_to<void> {
    template<typename T, typename U>
    constexpr auto operator()(T&& x, U&& y) const
        -> decltype(std::forward<T>(x) != std::forward<U>(y)) {
        return std::forward<T>(x) != std::forward<U>(y);
    }
};

template<typename T = void>
struct greater : binary_function<T, T, bool> {
    constexpr bool operator()(const T& x, const T& y) const {
        return x > y;
    }
};

template<>
struct greater<void> {
    template<typename T, typename U>
    constexpr auto operator()(T&& x, U&& y) const
        -> decltype(std::forward<T>(x) > std::forward<U>(y)) {
        return std::forward<T>(x) > std::forward<U>(y);
    }
};

template<typename T = void>
struct less : binary_function<T, T, bool> {
    constexpr bool operator()(const T& x, const T& y) const {
        return x < y;
    }
};

template<>
struct less<void> {
    template<typename T, typename U>
    constexpr auto operator()(T&& x, U&& y) const
        -> decltype(std::forward<T>(x) < std::forward<U>(y)) {
        return std::forward<T>(x) < std::forward<U>(y);
    }
};

template<typename T = void>
struct greater_equal : binary_function<T, T, bool> {
    constexpr bool operator()(const T& x, const T& y) const {
        return x >= y;
    }
};

template<>
struct greater_equal<void> {
    template<typename T, typename U>
    constexpr auto operator()(T&& x, U&& y) const
        -> decltype(std::forward<T>(x) >= std::forward<U>(y)) {
        return std::forward<T>(x) >= std::forward<U>(y);
    }
};

template<typename T = void>
struct less_equal : binary_function<T, T, bool> {
    constexpr bool operator()(const T& x, const T& y) const {
        return x <= y;
    }
};

template<>
struct less_equal<void> {
    template<typename T, typename U>
    constexpr auto operator()(T&& x, U&& y) const
        -> decltype(std::forward<T>(x) <= std::forward<U>(y)) {
        return std::forward<T>(x) <= std::forward<U>(y);
    }
};

// 逻辑运算符
template<typename T = void>
struct logical_and : binary_function<T, T, bool> {
    constexpr bool operator()(const T& x, const T& y) const {
        return x && y;
    }
};

template<>
struct logical_and<void> {
    template<typename T, typename U>
    constexpr auto operator()(T&& x, U&& y) const
        -> decltype(std::forward<T>(x) && std::forward<U>(y)) {
        return std::forward<T>(x) && std::forward<U>(y);
    }
};

template<typename T = void>
struct logical_or : binary_function<T, T, bool> {
    constexpr bool operator()(const T& x, const T& y) const {
        return x || y;
    }
};

template<>
struct logical_or<void> {
    template<typename T, typename U>
    constexpr auto operator()(T&& x, U&& y) const
        -> decltype(std::forward<T>(x) || std::forward<U>(y)) {
        return std::forward<T>(x) || std::forward<U>(y);
    }
};

template<typename T = void>
struct logical_not : unary_function<T, bool> {
    constexpr bool operator()(const T& x) const {
        return !x;
    }
};

template<>
struct logical_not<void> {
    template<typename T>
    constexpr auto operator()(T&& x) const
        -> decltype(!std::forward<T>(x)) {
        return !std::forward<T>(x);
    }
};

// 位运算符
template<typename T = void>
struct bit_and : binary_function<T, T, T> {
    constexpr T operator()(const T& x, const T& y) const {
        return x & y;
    }
};

template<>
struct bit_and<void> {
    template<typename T, typename U>
    constexpr auto operator()(T&& x, U&& y) const
        -> decltype(std::forward<T>(x) & std::forward<U>(y)) {
        return std::forward<T>(x) & std::forward<U>(y);
    }
};

template<typename T = void>
struct bit_or : binary_function<T, T, T> {
    constexpr T operator()(const T& x, const T& y) const {
        return x | y;
    }
};

template<>
struct bit_or<void> {
    template<typename T, typename U>
    constexpr auto operator()(T&& x, U&& y) const
        -> decltype(std::forward<T>(x) | std::forward<U>(y)) {
        return std::forward<T>(x) | std::forward<U>(y);
    }
};

template<typename T = void>
struct bit_xor : binary_function<T, T, T> {
    constexpr T operator()(const T& x, const T& y) const {
        return x ^ y;
    }
};

template<>
struct bit_xor<void> {
    template<typename T, typename U>
    constexpr auto operator()(T&& x, U&& y) const
        -> decltype(std::forward<T>(x) ^ std::forward<U>(y)) {
        return std::forward<T>(x) ^ std::forward<U>(y);
    }
};

template<typename T = void>
struct bit_not : unary_function<T, T> {
    constexpr T operator()(const T& x) const {
        return ~x;
    }
};

template<>
struct bit_not<void> {
    template<typename T>
    constexpr auto operator()(T&& x) const
        -> decltype(~std::forward<T>(x)) {
        return ~std::forward<T>(x);
    }
};

// 函数对象适配器 - 简化版本
template<typename Predicate>
class unary_negate {
public:
    explicit unary_negate(const Predicate& pred) : pred_(pred) {}
    
    template<typename T>
    bool operator()(const T& x) const {
        return !pred_(x);
    }
    
private:
    Predicate pred_;
};

template<typename Predicate>
unary_negate<Predicate> not1(const Predicate& pred) {
    return unary_negate<Predicate>(pred);
}

template<typename Predicate>
class binary_negate {
public:
    explicit binary_negate(const Predicate& pred) : pred_(pred) {}
    
    template<typename T, typename U>
    bool operator()(const T& x, const U& y) const {
        return !pred_(x, y);
    }
    
private:
    Predicate pred_;
};

template<typename Predicate>
binary_negate<Predicate> not2(const Predicate& pred) {
    return binary_negate<Predicate>(pred);
}

// 简化的绑定器 - 使用lambda (C++17 compatible)
template<typename Fn, typename... Args>
auto bind_front(Fn&& fn, Args&&... args) {
    return [fn = std::forward<Fn>(fn), args...] (auto&&... call_args) mutable -> decltype(fn(args..., std::forward<decltype(call_args)>(call_args)...)) {
        return fn(args..., std::forward<decltype(call_args)>(call_args)...);
    };
}

// 引用包装器
template<typename T>
class reference_wrapper {
public:
    using type = T;
    
    reference_wrapper() = delete;
    reference_wrapper(const reference_wrapper&) noexcept = default;
    reference_wrapper(reference_wrapper&&) noexcept = default;
    
    reference_wrapper(T& ref) noexcept : ptr_(&ref) {}
    
    reference_wrapper(T&&) = delete;
    
    reference_wrapper& operator=(const reference_wrapper&) noexcept = default;
    reference_wrapper& operator=(reference_wrapper&&) noexcept = default;
    
    operator T&() const noexcept { return *ptr_; }
    T& get() const noexcept { return *ptr_; }
    
    template<typename... ArgTypes>
    auto operator()(ArgTypes&&... args) const
        -> decltype(std::invoke(get(), std::forward<ArgTypes>(args)...)) {
        return std::invoke(get(), std::forward<ArgTypes>(args)...);
    }
    
private:
    T* ptr_;
};

template<typename T>
reference_wrapper<T> ref(T& t) noexcept {
    return reference_wrapper<T>(t);
}

template<typename T>
reference_wrapper<const T> cref(const T& t) noexcept {
    return reference_wrapper<const T>(t);
}

template<typename T>
reference_wrapper<T> ref(reference_wrapper<T> t) noexcept {
    return t;
}

template<typename T>
reference_wrapper<const T> cref(reference_wrapper<T> t) noexcept {
    return t;
}

// 成员函数适配器 - 简化版本，暂时注释掉
/*
template<typename R, typename C>
class mem_fn_t {
public:
    explicit mem_fn_t(R C::*pm) : pm_(pm) {}
    
    template<typename... Args>
    auto operator()(C* obj, Args&&... args) const
        -> decltype((obj->*pm_)(std::forward<Args>(args)...)) {
        return (obj->*pm_)(std::forward<Args>(args)...);
    }
    
    template<typename... Args>
    auto operator()(C& obj, Args&&... args) const
        -> decltype((obj.*pm_)(std::forward<Args>(args)...)) {
        return (obj.*pm_)(std::forward<Args>(args)...);
    }
    
    template<typename D, typename... Args>
    auto operator()(D&& obj, Args&&... args) const
        -> decltype((std::forward<D>(obj).*pm_)(std::forward<Args>(args)...)) {
        return (std::forward<D>(obj).*pm_)(std::forward<Args>(args)...);
    }
    
private:
    R C::*pm_;
};

template<typename R, typename C>
mem_fn_t<R, C> mem_fn(R C::*pm) {
    return mem_fn_t<R, C>(pm);
}
*/

// 哈希函数模板
template<typename T>
struct hash;

// 基本类型的哈希特化
template<>
struct hash<bool> {
    size_t operator()(bool x) const noexcept {
        return static_cast<size_t>(x);
    }
};

template<>
struct hash<char> {
    size_t operator()(char x) const noexcept {
        return static_cast<size_t>(x);
    }
};

template<>
struct hash<signed char> {
    size_t operator()(signed char x) const noexcept {
        return static_cast<size_t>(x);
    }
};

template<>
struct hash<unsigned char> {
    size_t operator()(unsigned char x) const noexcept {
        return static_cast<size_t>(x);
    }
};

template<>
struct hash<short> {
    size_t operator()(short x) const noexcept {
        return static_cast<size_t>(x);
    }
};

template<>
struct hash<unsigned short> {
    size_t operator()(unsigned short x) const noexcept {
        return static_cast<size_t>(x);
    }
};

template<>
struct hash<int> {
    size_t operator()(int x) const noexcept {
        return static_cast<size_t>(x);
    }
};

template<>
struct hash<unsigned int> {
    size_t operator()(unsigned int x) const noexcept {
        return static_cast<size_t>(x);
    }
};

template<>
struct hash<long> {
    size_t operator()(long x) const noexcept {
        return static_cast<size_t>(x);
    }
};

template<>
struct hash<unsigned long> {
    size_t operator()(unsigned long x) const noexcept {
        return static_cast<size_t>(x);
    }
};

template<>
struct hash<long long> {
    size_t operator()(long long x) const noexcept {
        return static_cast<size_t>(x);
    }
};

template<>
struct hash<unsigned long long> {
    size_t operator()(unsigned long long x) const noexcept {
        return static_cast<size_t>(x);
    }
};

template<>
struct hash<float> {
    size_t operator()(float x) const noexcept {
        return *reinterpret_cast<const size_t*>(&x);
    }
};

template<>
struct hash<double> {
    size_t operator()(double x) const noexcept {
        return *reinterpret_cast<const size_t*>(&x);
    }
};

template<>
struct hash<long double> {
    size_t operator()(long double x) const noexcept {
        return *reinterpret_cast<const size_t*>(&x);
    }
};

template<typename T>
struct hash<T*> {
    size_t operator()(T* p) const noexcept {
        return reinterpret_cast<size_t>(p);
    }
};

// std::string的哈希特化
template<>
struct hash<std::string> {
    size_t operator()(const std::string& str) const noexcept {
        size_t hash = 5381;
        for (char c : str) {
            hash = ((hash << 5) + hash) + c; // hash * 33 + c
        }
        return hash;
    }
};

// std::unique_ptr的哈希特化
template<typename T>
struct hash<std::unique_ptr<T>> {
    size_t operator()(const std::unique_ptr<T>& ptr) const noexcept {
        return hash<T*>()(ptr.get());
    }
};

// 恒等函数
template<typename T = void>
struct identity {
    constexpr T&& operator()(T&& x) const noexcept {
        return std::forward<T>(x);
    }
};

template<>
struct identity<void> {
    template<typename T>
    constexpr auto operator()(T&& x) const noexcept
        -> decltype(std::forward<T>(x)) {
        return std::forward<T>(x);
    }
};

} // namespace stl
#ifndef MY_FUNCTION_HPP
#define MY_FUNCTION_HPP

#include <iostream>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace my {

// 类型擦除的基础接口
template <typename>
class function;

// 特化版本：function<R(Args...)>
template <typename R, typename... Args>
class function<R(Args...)> {
private:
    // 抽象基类，用于类型擦除
    struct callable_base {
        virtual ~callable_base() = default;
        virtual R call(Args... args) = 0;
        virtual std::unique_ptr<callable_base> clone() const = 0;
    };

    // 具体实现模板，存储任意可调用对象
    template <typename F>
    struct callable_impl : callable_base {
        F f;

        callable_impl(F&& func) : f(std::forward<F>(func)) {}
        callable_impl(const F& func) : f(func) {}

        R call(Args... args) override {
            return f(std::forward<Args>(args)...);
        }

        std::unique_ptr<callable_base> clone() const override {
            return std::make_unique<callable_impl<F>>(f);
        }
    };

    std::unique_ptr<callable_base> callable_;

public:
    // 类型定义
    using result_type = R;

    // 构造函数
    function() noexcept = default;

    // 空指针构造
    function(std::nullptr_t) noexcept {}

    // 拷贝构造函数
    function(const function& other) {
        if (other.callable_) {
            callable_ = other.callable_->clone();
        }
    }

    // 移动构造函数
    function(function&& other) noexcept = default;

    // 构造函数：接受任意可调用对象
    template <typename F, typename = std::enable_if_t<
        !std::is_same_v<std::decay_t<F>, function> &&
        std::is_invocable_r_v<R, F, Args...>
    >>
    function(F&& f) {
        callable_ = std::make_unique<callable_impl<std::decay_t<F>>>(std::forward<F>(f));
    }

    // 析构函数
    ~function() = default;

    // 拷贝赋值运算符
    function& operator=(const function& other) {
        if (this != &other) {
            if (other.callable_) {
                callable_ = other.callable_->clone();
            } else {
                callable_.reset();
            }
        }
        return *this;
    }

    // 移动赋值运算符
    function& operator=(function&& other) noexcept = default;

    // 赋值运算符：接受任意可调用对象
    template <typename F, typename = std::enable_if_t<
        !std::is_same_v<std::decay_t<F>, function> &&
        std::is_invocable_r_v<R, F, Args...>
    >>
    function& operator=(F&& f) {
        callable_ = std::make_unique<callable_impl<std::decay_t<F>>>(std::forward<F>(f));
        return *this;
    }

    // 空指针赋值
    function& operator=(std::nullptr_t) noexcept {
        callable_.reset();
        return *this;
    }

    // 交换函数
    void swap(function& other) noexcept {
        callable_.swap(other.callable_);
    }

    // 调用运算符
    R operator()(Args... args) const {
        if (!callable_) {
            throw std::bad_function_call();
        }
        return callable_->call(std::forward<Args>(args)...);
    }

    // 检查是否为空
    explicit operator bool() const noexcept {
        return callable_ != nullptr;
    }

    // 获取目标类型的信息（简化版本）
    const std::type_info& target_type() const noexcept {
        if (!callable_) {
            return typeid(void);
        }
        // 返回一个非void类型表示有目标
        return typeid(callable_base);
    }

    // 获取目标指针（简化版本）
    template <typename T>
    T* target() noexcept {
        return nullptr; // 简化实现
    }

    template <typename T>
    const T* target() const noexcept {
        return nullptr; // 简化实现
    }
};

// 交换函数
template <typename R, typename... Args>
void swap(function<R(Args...)>& lhs, function<R(Args...)>& rhs) noexcept {
    lhs.swap(rhs);
}

// 比较运算符
template <typename R, typename... Args>
bool operator==(const function<R(Args...)>& f, std::nullptr_t) noexcept {
    return !f;
}

template <typename R, typename... Args>
bool operator==(std::nullptr_t, const function<R(Args...)>& f) noexcept {
    return !f;
}

template <typename R, typename... Args>
bool operator!=(const function<R(Args...)>& f, std::nullptr_t) noexcept {
    return f;
}

template <typename R, typename... Args>
bool operator!=(std::nullptr_t, const function<R(Args...)>& f) noexcept {
    return f;
}

// 辅助函数：创建function对象
template <typename R, typename... Args, typename F>
my::function<R(Args...)> make_function(F&& f) {
    return my::function<R(Args...)>(std::forward<F>(f));
}

} // namespace my

#endif // MY_FUNCTION_HPP
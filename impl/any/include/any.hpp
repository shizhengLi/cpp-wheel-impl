#ifndef MY_ANY_HPP
#define MY_ANY_HPP

#include <stdexcept>
#include <typeinfo>
#include <type_traits>
#include <utility>
#include <memory>
#include <new>

namespace my {

// bad_any_cast异常类
class bad_any_cast : public std::bad_cast {
public:
    bad_any_cast() noexcept = default;
    
    const char* what() const noexcept override {
        return "bad any cast";
    }
};

// 前向声明
class any;

// 类型擦除接口
class any_holder_base {
public:
    virtual ~any_holder_base() = default;
    virtual const std::type_info& type() const noexcept = 0;
    virtual any_holder_base* clone() const = 0;
    virtual void* get_ptr() noexcept = 0;
    virtual const void* get_ptr() const noexcept = 0;
};

// 具体类型实现
template <typename T>
class any_holder : public any_holder_base {
private:
    T value_;
    
public:
    template <typename U, typename = std::enable_if_t<
        !std::is_same_v<std::decay_t<U>, any_holder> &&
        std::is_constructible_v<T, U&&>
    >>
    explicit any_holder(U&& value) : value_(std::forward<U>(value)) {}
    
    const std::type_info& type() const noexcept override {
        return typeid(T);
    }
    
    any_holder_base* clone() const override {
        if constexpr (std::is_copy_constructible_v<T>) {
            return new any_holder(value_);
        } else {
            return nullptr; // 不可拷贝的类型无法clone
        }
    }
    
    void* get_ptr() noexcept override {
        return &value_;
    }
    
    const void* get_ptr() const noexcept override {
        return &value_;
    }
    
    T& get_value() noexcept {
        return value_;
    }
    
    const T& get_value() const noexcept {
        return value_;
    }
};

// any主类
class any {
private:
    std::unique_ptr<any_holder_base> holder_;
    
    template <typename T>
    T* cast_impl() noexcept {
        if (holder_ && holder_->type() == typeid(T)) {
            return &static_cast<any_holder<T>*>(holder_.get())->get_value();
        }
        return nullptr;
    }
    
    template <typename T>
    const T* cast_impl() const noexcept {
        if (holder_ && holder_->type() == typeid(T)) {
            return &static_cast<const any_holder<T>*>(holder_.get())->get_value();
        }
        return nullptr;
    }
    
public:
    // 构造函数
    any() noexcept = default;
    
    any(const any& other) {
        if (other.holder_) {
            holder_ = std::unique_ptr<any_holder_base>(other.holder_->clone());
        }
    }
    
    any(any&& other) noexcept = default;
    
    template <typename T, typename = std::enable_if_t<
        !std::is_same_v<std::decay_t<T>, any> &&
        !std::is_same_v<std::decay_t<T>, any_holder_base> &&
        (std::is_copy_constructible_v<std::decay_t<T>> || 
         std::is_move_constructible_v<std::decay_t<T>>)
    >>
    any(T&& value) {
        holder_ = std::make_unique<any_holder<std::decay_t<T>>>(std::forward<T>(value));
    }
    
    // 析构函数
    ~any() = default;
    
    // 赋值运算符
    any& operator=(const any& other) {
        if (this != &other) {
            if (other.holder_) {
                holder_ = std::unique_ptr<any_holder_base>(other.holder_->clone());
            } else {
                holder_.reset();
            }
        }
        return *this;
    }
    
    any& operator=(any&& other) noexcept = default;
    
    template <typename T, typename = std::enable_if_t<
        !std::is_same_v<std::decay_t<T>, any> &&
        (std::is_copy_constructible_v<std::decay_t<T>> || 
         std::is_move_constructible_v<std::decay_t<T>>)
    >>
    any& operator=(T&& value) {
        holder_ = std::make_unique<any_holder<std::decay_t<T>>>(std::forward<T>(value));
        return *this;
    }
    
    // 修改操作
    void reset() noexcept {
        holder_.reset();
    }
    
    void swap(any& other) noexcept {
        holder_.swap(other.holder_);
    }
    
    // 观察操作
    bool has_value() const noexcept {
        return holder_ != nullptr;
    }
    
    const std::type_info& type() const noexcept {
        return holder_ ? holder_->type() : typeid(void);
    }
    
    // 类型转换
    template <typename T>
    T* cast() noexcept {
        return cast_impl<T>();
    }
    
    template <typename T>
    const T* cast() const noexcept {
        return cast_impl<T>();
    }
};

// 非成员函数
template <typename T>
T any_cast(const any& operand) {
    using U = std::remove_cv_t<std::remove_reference_t<T>>;
    static_assert(std::is_constructible_v<T, const U&>, "Target type must be constructible from source type");
    
    if (auto ptr = operand.template cast<U>()) {
        return static_cast<T>(*ptr);
    }
    throw bad_any_cast();
}

template <typename T>
T any_cast(any& operand) {
    using U = std::remove_cv_t<std::remove_reference_t<T>>;
    static_assert(std::is_constructible_v<T, U&>, "Target type must be constructible from source type");
    
    if (auto ptr = operand.template cast<U>()) {
        return static_cast<T>(*ptr);
    }
    throw bad_any_cast();
}

template <typename T>
T any_cast(any&& operand) {
    using U = std::remove_cv_t<std::remove_reference_t<T>>;
    static_assert(std::is_constructible_v<T, U>, "Target type must be constructible from source type");
    
    if (auto ptr = operand.template cast<U>()) {
        return static_cast<T>(std::move(*ptr));
    }
    throw bad_any_cast();
}

template <typename T>
const T* any_cast(const any* operand) noexcept {
    if (!operand) {
        return nullptr;
    }
    return operand->template cast<T>();
}

template <typename T>
T* any_cast(any* operand) noexcept {
    if (!operand) {
        return nullptr;
    }
    return operand->template cast<T>();
}

// make_any辅助函数
template <typename T, typename... Args>
any make_any(Args&&... args) {
    return any(T(std::forward<Args>(args)...));
}

// swap函数
void swap(any& lhs, any& rhs) noexcept {
    lhs.swap(rhs);
}

} // namespace my

#endif // MY_ANY_HPP
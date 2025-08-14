#ifndef MY_OPTIONAL_HPP
#define MY_OPTIONAL_HPP

#include <stdexcept>
#include <type_traits>
#include <utility>
#include <new>

namespace my {

// nullopt_t类型和nullopt对象
struct nullopt_t {
    constexpr explicit nullopt_t(int) {}
};

inline constexpr nullopt_t nullopt{0};

// bad_optional_access异常类
class bad_optional_access : public std::bad_optional_access {
public:
    bad_optional_access() : std::bad_optional_access() {}
    const char* what() const noexcept override {
        return "bad optional access";
    }
};

// optional主类
template <typename T>
class optional {
private:
    // 使用联合体来管理存储
    union {
        T value_;
        char dummy_;
    };
    
    bool has_value_;
    
    // 构造值的辅助函数
    template <typename... Args>
    void construct_value(Args&&... args) {
        new (&value_) T(std::forward<Args>(args)...);
        has_value_ = true;
    }
    
    // 销毁值的辅助函数
    void destroy_value() {
        if (has_value_) {
            value_.~T();
            has_value_ = false;
        }
    }
    
public:
    // 类型定义
    using value_type = T;
    
    // 构造函数
    constexpr optional() noexcept : has_value_(false) {}
    
    constexpr optional(nullopt_t) noexcept : has_value_(false) {}
    
    // 拷贝构造函数
    optional(const optional& other) : has_value_(false) {
        if (other.has_value_) {
            construct_value(other.value_);
        }
    }
    
    // 移动构造函数
    optional(optional&& other) noexcept : has_value_(false) {
        if (other.has_value_) {
            construct_value(std::move(other.value_));
        }
    }
    
    // 值构造函数
    template <typename U = T, typename = std::enable_if_t<
        !std::is_same_v<std::decay_t<U>, optional> &&
        std::is_constructible_v<T, U&&>
    >>
    optional(U&& value) : has_value_(false) {
        construct_value(std::forward<U>(value));
    }
    
    // in_place构造函数
    template <typename... Args>
    constexpr explicit optional(std::in_place_t, Args&&... args) : has_value_(false) {
        construct_value(std::forward<Args>(args)...);
    }
    
    template <typename U, typename... Args>
    constexpr explicit optional(std::in_place_t, std::initializer_list<U> ilist, Args&&... args) : has_value_(false) {
        construct_value(ilist, std::forward<Args>(args)...);
    }
    
    // 析构函数
    ~optional() {
        destroy_value();
    }
    
    // 拷贝赋值运算符
    optional& operator=(const optional& other) {
        if (this != &other) {
            if (other.has_value_) {
                if (has_value_) {
                    value_ = other.value_;
                } else {
                    construct_value(other.value_);
                }
            } else {
                destroy_value();
            }
        }
        return *this;
    }
    
    // 移动赋值运算符
    optional& operator=(optional&& other) noexcept {
        if (this != &other) {
            if (other.has_value_) {
                if (has_value_) {
                    value_ = std::move(other.value_);
                } else {
                    construct_value(std::move(other.value_));
                }
            } else {
                destroy_value();
            }
        }
        return *this;
    }
    
    // 值赋值运算符
    template <typename U = T, typename = std::enable_if_t<
        !std::is_same_v<std::decay_t<U>, optional> &&
        std::is_constructible_v<T, U&&>
    >>
    optional& operator=(U&& value) {
        if (has_value_) {
            value_ = std::forward<U>(value);
        } else {
            construct_value(std::forward<U>(value));
        }
        return *this;
    }
    
    // nullopt赋值
    optional& operator=(nullopt_t) noexcept {
        destroy_value();
        return *this;
    }
    
    // 检查是否有值
    constexpr explicit operator bool() const noexcept {
        return has_value_;
    }
    
    constexpr bool has_value() const noexcept {
        return has_value_;
    }
    
    // 值访问
    constexpr const T& operator*() const& noexcept {
        return value_;
    }
    
    constexpr T& operator*() & noexcept {
        return value_;
    }
    
    constexpr const T&& operator*() const&& noexcept {
        return std::move(value_);
    }
    
    constexpr T&& operator*() && noexcept {
        return std::move(value_);
    }
    
    constexpr const T* operator->() const noexcept {
        return &value_;
    }
    
    constexpr T* operator->() noexcept {
        return &value_;
    }
    
    constexpr const T& value() const& {
        if (!has_value_) {
            throw bad_optional_access();
        }
        return value_;
    }
    
    constexpr T& value() & {
        if (!has_value_) {
            throw bad_optional_access();
        }
        return value_;
    }
    
    constexpr const T&& value() const&& {
        if (!has_value_) {
            throw bad_optional_access();
        }
        return std::move(value_);
    }
    
    constexpr T&& value() && {
        if (!has_value_) {
            throw bad_optional_access();
        }
        return std::move(value_);
    }
    
    // value_or方法
    template <typename U>
    constexpr T value_or(U&& default_value) const& {
        return has_value_ ? value_ : static_cast<T>(std::forward<U>(default_value));
    }
    
    template <typename U>
    constexpr T value_or(U&& default_value) && {
        return has_value_ ? std::move(value_) : static_cast<T>(std::forward<U>(default_value));
    }
    
    // emplace方法
    template <typename... Args>
    T& emplace(Args&&... args) {
        destroy_value();
        construct_value(std::forward<Args>(args)...);
        return value_;
    }
    
    template <typename U, typename... Args>
    T& emplace(std::initializer_list<U> ilist, Args&&... args) {
        destroy_value();
        construct_value(ilist, std::forward<Args>(args)...);
        return value_;
    }
    
    // reset方法
    void reset() noexcept {
        destroy_value();
    }
    
    // swap方法
    void swap(optional& other) noexcept {
        if (has_value_ && other.has_value_) {
            using std::swap;
            swap(value_, other.value_);
        } else if (has_value_) {
            other.construct_value(std::move(value_));
            destroy_value();
        } else if (other.has_value_) {
            construct_value(std::move(other.value_));
            other.destroy_value();
        }
    }
};

// 比较运算符
template <typename T>
constexpr bool operator==(const optional<T>& lhs, const optional<T>& rhs) {
    if (lhs.has_value() != rhs.has_value()) {
        return false;
    }
    if (!lhs.has_value()) {
        return true;
    }
    return lhs.value() == rhs.value();
}

template <typename T>
constexpr bool operator!=(const optional<T>& lhs, const optional<T>& rhs) {
    return !(lhs == rhs);
}

template <typename T>
constexpr bool operator<(const optional<T>& lhs, const optional<T>& rhs) {
    if (!rhs.has_value()) {
        return false;
    }
    if (!lhs.has_value()) {
        return true;
    }
    return lhs.value() < rhs.value();
}

template <typename T>
constexpr bool operator<=(const optional<T>& lhs, const optional<T>& rhs) {
    return !(rhs < lhs);
}

template <typename T>
constexpr bool operator>(const optional<T>& lhs, const optional<T>& rhs) {
    return rhs < lhs;
}

template <typename T>
constexpr bool operator>=(const optional<T>& lhs, const optional<T>& rhs) {
    return !(lhs < rhs);
}

// 与nullopt的比较
template <typename T>
constexpr bool operator==(const optional<T>& opt, nullopt_t) noexcept {
    return !opt.has_value();
}

template <typename T>
constexpr bool operator==(nullopt_t, const optional<T>& opt) noexcept {
    return !opt.has_value();
}

template <typename T>
constexpr bool operator!=(const optional<T>& opt, nullopt_t) noexcept {
    return opt.has_value();
}

template <typename T>
constexpr bool operator!=(nullopt_t, const optional<T>& opt) noexcept {
    return opt.has_value();
}

template <typename T>
constexpr bool operator<(const optional<T>&, nullopt_t) noexcept {
    return false;
}

template <typename T>
constexpr bool operator<(nullopt_t, const optional<T>& opt) noexcept {
    return opt.has_value();
}

template <typename T>
constexpr bool operator<=(const optional<T>& opt, nullopt_t) noexcept {
    return !opt.has_value();
}

template <typename T>
constexpr bool operator<=(nullopt_t, const optional<T>&) noexcept {
    return true;
}

template <typename T>
constexpr bool operator>(const optional<T>& opt, nullopt_t) noexcept {
    return opt.has_value();
}

template <typename T>
constexpr bool operator>(nullopt_t, const optional<T>&) noexcept {
    return false;
}

template <typename T>
constexpr bool operator>=(const optional<T>&, nullopt_t) noexcept {
    return true;
}

template <typename T>
constexpr bool operator>=(nullopt_t, const optional<T>& opt) noexcept {
    return !opt.has_value();
}

// 与值的比较
template <typename T, typename U>
constexpr bool operator==(const optional<T>& opt, const U& value) {
    return opt.has_value() ? opt.value() == value : false;
}

template <typename T, typename U>
constexpr bool operator==(const U& value, const optional<T>& opt) {
    return opt.has_value() ? value == opt.value() : false;
}

template <typename T, typename U>
constexpr bool operator!=(const optional<T>& opt, const U& value) {
    return !(opt == value);
}

template <typename T, typename U>
constexpr bool operator!=(const U& value, const optional<T>& opt) {
    return !(value == opt);
}

template <typename T, typename U>
constexpr bool operator<(const optional<T>& opt, const U& value) {
    return opt.has_value() ? opt.value() < value : true;
}

template <typename T, typename U>
constexpr bool operator<(const U& value, const optional<T>& opt) {
    return opt.has_value() ? value < opt.value() : false;
}

template <typename T, typename U>
constexpr bool operator<=(const optional<T>& opt, const U& value) {
    return !(value < opt);
}

template <typename T, typename U>
constexpr bool operator<=(const U& value, const optional<T>& opt) {
    return !(opt < value);
}

template <typename T, typename U>
constexpr bool operator>(const optional<T>& opt, const U& value) {
    return value < opt;
}

template <typename T, typename U>
constexpr bool operator>(const U& value, const optional<T>& opt) {
    return opt < value;
}

template <typename T, typename U>
constexpr bool operator>=(const optional<T>& opt, const U& value) {
    return !(opt < value);
}

template <typename T, typename U>
constexpr bool operator>=(const U& value, const optional<T>& opt) {
    return !(value < opt);
}

// swap函数
template <typename T>
void swap(optional<T>& lhs, optional<T>& rhs) noexcept {
    lhs.swap(rhs);
}

// make_optional辅助函数
template <typename T>
constexpr optional<std::decay_t<T>> make_optional(T&& value) {
    return optional<std::decay_t<T>>(std::forward<T>(value));
}

template <typename T, typename... Args>
constexpr optional<T> make_optional(Args&&... args) {
    return optional<T>(std::in_place, std::forward<Args>(args)...);
}

template <typename T, typename U, typename... Args>
constexpr optional<T> make_optional(std::initializer_list<U> ilist, Args&&... args) {
    return optional<T>(std::in_place, ilist, std::forward<Args>(args)...);
}

} // namespace my

#endif // MY_OPTIONAL_HPP
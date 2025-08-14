#ifndef MY_VARIANT_HPP
#define MY_VARIANT_HPP

#include <stdexcept>
#include <type_traits>
#include <utility>
#include <new>
#include <cstddef>

namespace my {

// variant_npos常量
inline constexpr size_t variant_npos = static_cast<size_t>(-1);

// bad_variant_access异常类
class bad_variant_access : public std::exception {
public:
    bad_variant_access() noexcept = default;
    
    const char* what() const noexcept override {
        return "bad variant access";
    }
};

// 辅助类型特征
template <typename T, typename... Types>
struct index_of;

template <typename T, typename First, typename... Rest>
struct index_of<T, First, Rest...> {
    static constexpr size_t value = 
        std::is_same_v<T, First> ? 0 : index_of<T, Rest...>::value + 1;
};

template <typename T>
struct index_of<T> {
    static constexpr size_t value = variant_npos;
};

template <typename T, typename... Types>
inline constexpr size_t index_of_v = index_of<T, Types...>::value;

// 检查类型是否在类型列表中
template <typename T, typename... Types>
struct contains_type : std::disjunction<std::is_same<T, Types>...> {};

template <typename T, typename... Types>
inline constexpr bool contains_type_v = contains_type<T, Types...>::value;

// 检查类型是否唯一
template <typename T, typename... Types>
struct is_unique_type;

template <typename T>
struct is_unique_type<T> : std::true_type {};

template <typename T, typename First, typename... Rest>
struct is_unique_type<T, First, Rest...> {
    static constexpr bool value = 
        !std::is_same_v<T, First> && is_unique_type<T, Rest...>::value;
};

template <typename... Types>
struct are_types_unique;

template <>
struct are_types_unique<> : std::true_type {};

template <typename First, typename... Rest>
struct are_types_unique<First, Rest...> {
    static constexpr bool value = 
        is_unique_type<First, Rest...>::value && are_types_unique<Rest...>::value;
};

template <typename... Types>
inline constexpr bool are_types_unique_v = are_types_unique<Types...>::value;

// 获取第N个类型
template <size_t N, typename... Types>
struct type_at;

template <size_t N, typename First, typename... Rest>
struct type_at<N, First, Rest...> {
    using type = typename type_at<N - 1, Rest...>::type;
};

template <typename First, typename... Rest>
struct type_at<0, First, Rest...> {
    using type = First;
};

template <size_t N, typename... Types>
using type_at_t = typename type_at<N, Types...>::type;

// variant_storage类
template <size_t Index, typename... Types>
struct variant_storage;

template <size_t Index>
struct variant_storage<Index> {
    void destroy(size_t) {}
    void copy_from(const variant_storage&, size_t) {}
    void move_from(variant_storage&&, size_t) {}
    
    template <typename T>
    T& get(size_t) {
        throw bad_variant_access();
    }
    
    template <typename T>
    const T& get(size_t) const {
        throw bad_variant_access();
    }
};

template <size_t Index, typename First, typename... Rest>
struct variant_storage<Index, First, Rest...> {
    union {
        First value;
        variant_storage<Index + 1, Rest...> rest;
    };
    
    variant_storage() {}
    
    ~variant_storage() {}
    
    template <typename T, typename... Args>
    void construct(size_t target_index, Args&&... args) {
        if constexpr (std::is_same_v<T, First>) {
            if (target_index == Index) {
                new (&value) First(std::forward<Args>(args)...);
            }
        } else {
            rest.template construct<T>(target_index, std::forward<Args>(args)...);
        }
    }
    
    void destroy(size_t current_index) {
        if (current_index == Index) {
            value.~First();
        } else {
            rest.destroy(current_index);
        }
    }
    
    template <typename T>
    T& get(size_t current_index) {
        if constexpr (std::is_same_v<T, First>) {
            if (current_index == Index) {
                return value;
            }
        }
        return rest.template get<T>(current_index);
    }
    
    template <typename T>
    const T& get(size_t current_index) const {
        if constexpr (std::is_same_v<T, First>) {
            if (current_index == Index) {
                return value;
            }
        }
        return rest.template get<T>(current_index);
    }
    
    void copy_from(const variant_storage& other, size_t current_index) {
        if (current_index == Index) {
            new (&value) First(other.value);
        } else {
            rest.copy_from(other.rest, current_index);
        }
    }
    
    void move_from(variant_storage&& other, size_t current_index) {
        if (current_index == Index) {
            new (&value) First(std::move(other.value));
        } else {
            rest.move_from(std::move(other.rest), current_index);
        }
    }
};

// variant主类
template <typename... Types>
class variant {
    static_assert(sizeof...(Types) > 0, "variant must have at least one type");
    static_assert(are_types_unique_v<Types...>, "variant types must be unique");
    
    variant_storage<0, Types...> storage_;
    size_t index_;
    
    template <typename T>
    static constexpr size_t type_index() {
        return index_of_v<T, Types...>;
    }
    
    template <typename T>
    void construct_value(T&& value) {
        using Type = std::decay_t<T>;
        static_assert(contains_type_v<Type, Types...>, "Type not in variant");
        
        index_ = type_index<Type>();
        storage_.template construct<Type>(index_, std::forward<T>(value));
    }
    
    void destroy_value() {
        if (index_ != variant_npos) {
            storage_.destroy(index_);
            index_ = variant_npos;
        }
    }
    
public:
    // 构造函数
    variant() : index_(0) {
        storage_.template construct<type_at_t<0, Types...>>(0, type_at_t<0, Types...>());
    }
    
    template <typename T, typename = std::enable_if_t<
        !std::is_same_v<std::decay_t<T>, variant> &&
        contains_type_v<std::decay_t<T>, Types...>
    >>
    variant(T&& value) {
        construct_value(std::forward<T>(value));
    }
    
    // 拷贝构造函数
    variant(const variant& other) : index_(other.index_) {
        if (index_ != variant_npos) {
            storage_.copy_from(other.storage_, index_);
        }
    }
    
    // 移动构造函数
    variant(variant&& other) noexcept : index_(other.index_) {
        if (index_ != variant_npos) {
            storage_.move_from(std::move(other.storage_), index_);
        }
    }
    
    // 析构函数
    ~variant() {
        destroy_value();
    }
    
    // 赋值运算符
    variant& operator=(const variant& other) {
        if (this != &other) {
            destroy_value();
            index_ = other.index_;
            if (index_ != variant_npos) {
                storage_.copy_from(other.storage_, index_);
            }
        }
        return *this;
    }
    
    variant& operator=(variant&& other) noexcept {
        if (this != &other) {
            destroy_value();
            index_ = other.index_;
            if (index_ != variant_npos) {
                storage_.move_from(std::move(other.storage_), index_);
            }
        }
        return *this;
    }
    
    template <typename T, typename = std::enable_if_t<
        !std::is_same_v<std::decay_t<T>, variant> &&
        contains_type_v<std::decay_t<T>, Types...>
    >>
    variant& operator=(T&& value) {
        destroy_value();
        construct_value(std::forward<T>(value));
        return *this;
    }
    
    // 访问操作
    size_t index() const noexcept {
        return index_;
    }
    
    bool valueless_by_exception() const noexcept {
        return index_ == variant_npos;
    }
    
    template <typename T>
    bool holds_alternative() const noexcept {
        return index_ == type_index<T>();
    }
    
    template <typename T>
    const T& get() const {
        if (!holds_alternative<T>()) {
            throw bad_variant_access();
        }
        return storage_.template get<T>(index_);
    }
    
    template <typename T>
    T& get() {
        if (!holds_alternative<T>()) {
            throw bad_variant_access();
        }
        return storage_.template get<T>(index_);
    }
    
    template <size_t I>
    const type_at_t<I, Types...>& get() const {
        return get<type_at_t<I, Types...>>();
    }
    
    template <size_t I>
    type_at_t<I, Types...>& get() {
        return get<type_at_t<I, Types...>>();
    }
    
    // emplace操作
    template <typename T, typename... Args>
    T& emplace(Args&&... args) {
        destroy_value();
        index_ = type_index<T>();
        storage_.template construct<T>(index_, std::forward<Args>(args)...);
        return storage_.template get<T>(index_);
    }
    
    template <size_t I, typename... Args>
    type_at_t<I, Types...>& emplace(Args&&... args) {
        return emplace<type_at_t<I, Types...>>(std::forward<Args>(args)...);
    }
    
    // swap操作
    void swap(variant& other) noexcept {
        if (index_ == other.index_) {
            if (index_ != variant_npos) {
                // 相同类型，交换值
                variant temp(std::move(*this));
                *this = std::move(other);
                other = std::move(temp);
            }
        } else {
            variant temp(std::move(*this));
            *this = std::move(other);
            other = std::move(temp);
        }
    }
};

// 辅助函数
template <typename T, typename... Types>
constexpr bool holds_alternative(const variant<Types...>& v) noexcept {
    return v.template holds_alternative<T>();
}

template <size_t I, typename... Types>
constexpr auto& get(variant<Types...>& v) {
    return v.template get<I>();
}

template <size_t I, typename... Types>
constexpr const auto& get(const variant<Types...>& v) {
    return v.template get<I>();
}

template <typename T, typename... Types>
constexpr auto& get(variant<Types...>& v) {
    return v.template get<T>();
}

template <typename T, typename... Types>
constexpr const auto& get(const variant<Types...>& v) {
    return v.template get<T>();
}

// 比较运算符
template <typename... Types>
bool operator==(const variant<Types...>& lhs, const variant<Types...>& rhs) {
    if (lhs.index() != rhs.index()) {
        return false;
    }
    if (lhs.valueless_by_exception()) {
        return true;
    }
    
    // 简化实现：只支持基本类型比较
    if constexpr (sizeof...(Types) > 0) {
        if (lhs.index() == 0) {
            return my::get<0>(lhs) == my::get<0>(rhs);
        }
    }
    if constexpr (sizeof...(Types) > 1) {
        if (lhs.index() == 1) {
            return my::get<1>(lhs) == my::get<1>(rhs);
        }
    }
    if constexpr (sizeof...(Types) > 2) {
        if (lhs.index() == 2) {
            return my::get<2>(lhs) == my::get<2>(rhs);
        }
    }
    
    return false;
}

template <typename... Types>
bool operator!=(const variant<Types...>& lhs, const variant<Types...>& rhs) {
    return !(lhs == rhs);
}

// visit函数模板
template <typename Visitor, typename... Variants>
decltype(auto) visit(Visitor&& vis, Variants&&... vars) {
    // 简化实现，实际需要更复杂的访问器模式
    throw bad_variant_access();
}

} // namespace my

#endif // MY_VARIANT_HPP
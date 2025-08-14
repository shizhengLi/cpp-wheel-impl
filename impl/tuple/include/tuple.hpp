#ifndef MY_TUPLE_HPP
#define MY_TUPLE_HPP

#include <utility>
#include <type_traits>

namespace my {

// 前向声明
template <typename... Types>
class tuple;

// tuple_size 特化
template <typename T>
struct tuple_size;

template <typename... Types>
struct tuple_size<tuple<Types...>> 
    : std::integral_constant<size_t, sizeof...(Types)> {};

template <typename T>
inline constexpr size_t tuple_size_v = tuple_size<T>::value;

// tuple_element 特化
template <size_t I, typename T>
struct tuple_element;

template <size_t I, typename Head, typename... Tail>
struct tuple_element<I, tuple<Head, Tail...>> 
    : tuple_element<I - 1, tuple<Tail...>> {};

template <typename Head, typename... Tail>
struct tuple_element<0, tuple<Head, Tail...>> {
    using type = Head;
};

template <size_t I, typename T>
using tuple_element_t = typename tuple_element<I, T>::type;

// 基础 tuple 实现递归终止
template <>
class tuple<> {
public:
    tuple() = default;
    tuple(const tuple&) = default;
    tuple(tuple&&) = default;
    tuple& operator=(const tuple&) = default;
    tuple& operator=(tuple&&) = default;
    
    void swap(tuple& other) {
        // 空tuple，无需操作
    }
};

// tuple 主实现
template <typename Head, typename... Tail>
class tuple<Head, Tail...> {
private:
    Head head_;
    tuple<Tail...> tail_;
    
public:
    // 默认构造
    tuple() = default;
    
    // 拷贝构造
    tuple(const tuple&) = default;
    
    // 移动构造
    tuple(tuple&&) = default;
    
    // 值构造
    explicit tuple(const Head& head, const Tail&... tail) 
        : head_(head), tail_(tail...) {}
    
    // 完美转发构造
    template <typename U, typename... Us>
    explicit tuple(U&& u, Us&&... us) 
        : head_(std::forward<U>(u)), tail_(std::forward<Us>(us)...) {}
    
    // 拷贝赋值
    tuple& operator=(const tuple&) = default;
    
    // 移动赋值
    tuple& operator=(tuple&&) = default;
    
    // 获取头元素
    Head& head() noexcept { return head_; }
    const Head& head() const noexcept { return head_; }
    
    // 获取尾部
    auto& tail() noexcept { return tail_; }
    const auto& tail() const noexcept { return tail_; }
    
    // swap 函数
    void swap(tuple& other) {
        if constexpr (sizeof...(Tail) > 0) {
            using std::swap;
            swap(head_, other.head_);
            tail_.swap(other.tail_);
        } else {
            using std::swap;
            swap(head_, other.head_);
        }
    }
};

// make_tuple 辅助函数
template <typename... Types>
constexpr auto make_tuple(Types&&... args) {
    return tuple<std::decay_t<Types>...>(std::forward<Types>(args)...);
}

// get 函数模板
namespace detail {

// 递归获取元素
template <size_t I, typename Tuple>
constexpr auto& get_impl(Tuple& t) noexcept {
    if constexpr (I == 0) {
        return t.head();
    } else {
        return get_impl<I - 1>(t.tail());
    }
}

template <size_t I, typename Tuple>
constexpr const auto& get_impl(const Tuple& t) noexcept {
    if constexpr (I == 0) {
        return t.head();
    } else {
        return get_impl<I - 1>(t.tail());
    }
}

template <size_t I, typename Tuple>
constexpr auto&& get_impl(Tuple&& t) noexcept {
    if constexpr (I == 0) {
        return std::forward<decltype(t.head())>(t.head());
    } else {
        return get_impl<I - 1>(std::forward<decltype(t.tail())>(t.tail()));
    }
}

} // namespace detail

// get 函数
template <size_t I, typename... Types>
constexpr auto& get(tuple<Types...>& t) noexcept {
    static_assert(I < sizeof...(Types), "tuple index out of range");
    return detail::get_impl<I>(t);
}

template <size_t I, typename... Types>
constexpr const auto& get(const tuple<Types...>& t) noexcept {
    static_assert(I < sizeof...(Types), "tuple index out of range");
    return detail::get_impl<I>(t);
}

template <size_t I, typename... Types>
constexpr auto&& get(tuple<Types...>&& t) noexcept {
    static_assert(I < sizeof...(Types), "tuple index out of range");
    return detail::get_impl<I>(std::move(t));
}

// 类型获取
template <typename T, typename... Types>
constexpr T& get(tuple<Types...>& t) noexcept {
    static_assert((std::is_same_v<T, Types> || ...), "type not in tuple");
    // 简化实现：线性搜索
    if constexpr (std::is_same_v<T, tuple_element_t<0, tuple<Types...>>>) {
        return get<0>(t);
    } else if constexpr (sizeof...(Types) > 1) {
        return get<T>(t.tail());
    }
}

template <typename T, typename... Types>
constexpr const T& get(const tuple<Types...>& t) noexcept {
    static_assert((std::is_same_v<T, Types> || ...), "type not in tuple");
    if constexpr (std::is_same_v<T, tuple_element_t<0, tuple<Types...>>>) {
        return get<0>(t);
    } else if constexpr (sizeof...(Types) > 1) {
        return get<T>(t.tail());
    }
}

template <typename T, typename... Types>
constexpr T&& get(tuple<Types...>&& t) noexcept {
    static_assert((std::is_same_v<T, Types> || ...), "type not in tuple");
    if constexpr (std::is_same_v<T, tuple_element_t<0, tuple<Types...>>>) {
        return get<0>(std::move(t));
    } else if constexpr (sizeof...(Types) > 1) {
        return get<T>(std::move(t.tail()));
    }
}

// tuple_cat 连接函数
namespace detail {

// 递归连接
template <typename... Tuples>
struct tuple_cat_result;

template <typename Tuple>
struct tuple_cat_result<Tuple> {
    using type = Tuple;
};

template <typename Tuple1, typename Tuple2>
struct tuple_cat_result<Tuple1, Tuple2> {
    // 简化实现：只处理基本连接
    using type = Tuple1;
};

template <typename... Tuples>
using tuple_cat_result_t = typename tuple_cat_result<Tuples...>::type;

} // namespace detail

template <typename... Tuples>
constexpr auto tuple_cat(Tuples&&... tuples) {
    // 简化实现：只支持单个tuple
    static_assert(sizeof...(Tuples) == 1, "tuple_cat not fully implemented");
    return std::forward<Tuples...>(tuples...);
}

// tie 函数
template <typename... Types>
constexpr tuple<Types&...> tie(Types&... args) noexcept {
    return tuple<Types&...>(args...);
}

// forward_as_tuple
template <typename... Types>
constexpr tuple<Types&&...> forward_as_tuple(Types&&... args) noexcept {
    return tuple<Types&&...>(std::forward<Types>(args)...);
}

// 比较运算符
template <typename... Types1, typename... Types2>
constexpr bool operator==(const tuple<Types1...>& t1, const tuple<Types2...>& t2) {
    static_assert(sizeof...(Types1) == sizeof...(Types2), "tuple size mismatch");
    if constexpr (sizeof...(Types1) == 0) {
        return true;
    } else if constexpr (sizeof...(Types1) == 1) {
        return get<0>(t1) == get<0>(t2);
    } else {
        return get<0>(t1) == get<0>(t2) && 
               t1.tail() == t2.tail();
    }
}

template <typename... Types1, typename... Types2>
constexpr bool operator!=(const tuple<Types1...>& t1, const tuple<Types2...>& t2) {
    return !(t1 == t2);
}

template <typename... Types1, typename... Types2>
constexpr bool operator<(const tuple<Types1...>& t1, const tuple<Types2...>& t2) {
    static_assert(sizeof...(Types1) == sizeof...(Types2), "tuple size mismatch");
    if constexpr (sizeof...(Types1) == 0) {
        return false;
    } else {
        if (get<0>(t1) < get<0>(t2)) return true;
        if (get<0>(t2) < get<0>(t1)) return false;
        if constexpr (sizeof...(Types1) > 1) {
            return t1.tail() < t2.tail();
        } else {
            return false;
        }
    }
}

template <typename... Types1, typename... Types2>
constexpr bool operator<=(const tuple<Types1...>& t1, const tuple<Types2...>& t2) {
    return !(t2 < t1);
}

template <typename... Types1, typename... Types2>
constexpr bool operator>(const tuple<Types1...>& t1, const tuple<Types2...>& t2) {
    return t2 < t1;
}

template <typename... Types1, typename... Types2>
constexpr bool operator>=(const tuple<Types1...>& t1, const tuple<Types2...>& t2) {
    return !(t1 < t2);
}

// swap 函数
template <typename... Types>
void swap(tuple<Types...>& t1, tuple<Types...>& t2) {
    using std::swap;
    if constexpr (sizeof...(Types) > 0) {
        swap(get<0>(t1), get<0>(t2));
        if constexpr (sizeof...(Types) > 1) {
            swap(t1.tail(), t2.tail());
        }
    }
}

} // namespace my

#endif // MY_TUPLE_HPP
#ifndef STL_ITERATOR_HPP
#define STL_ITERATOR_HPP

#include <cstddef>
#include <iterator>
#include <type_traits>

namespace stl {

// 迭代器类别标签
struct input_iterator_tag {};
struct output_iterator_tag {};
struct forward_iterator_tag : public input_iterator_tag {};
struct bidirectional_iterator_tag : public forward_iterator_tag {};
struct random_access_iterator_tag : public bidirectional_iterator_tag {};

// 迭代器特性萃取器
template <typename Iterator>
struct iterator_traits {
    using iterator_category = typename Iterator::iterator_category;
    using value_type = typename Iterator::value_type;
    using difference_type = typename Iterator::difference_type;
    using pointer = typename Iterator::pointer;
    using reference = typename Iterator::reference;
};

// 指针特化
template <typename T>
struct iterator_traits<T*> {
    using iterator_category = random_access_iterator_tag;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = T*;
    using reference = T&;
};

// const指针特化
template <typename T>
struct iterator_traits<const T*> {
    using iterator_category = random_access_iterator_tag;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = const T*;
    using reference = const T&;
};

// 迭代器基类
template <typename Category, typename T, typename Distance = std::ptrdiff_t,
          typename Pointer = T*, typename Reference = T&>
struct iterator {
    using iterator_category = Category;
    using value_type = T;
    using difference_type = Distance;
    using pointer = Pointer;
    using reference = Reference;
};

// 反向迭代器
template <typename Iterator>
class reverse_iterator {
public:
    using iterator_type = Iterator;
    using iterator_category = typename iterator_traits<Iterator>::iterator_category;
    using value_type = typename iterator_traits<Iterator>::value_type;
    using difference_type = typename iterator_traits<Iterator>::difference_type;
    using pointer = typename iterator_traits<Iterator>::pointer;
    using reference = typename iterator_traits<Iterator>::reference;

    // 构造函数
    reverse_iterator() : current_() {}
    
    explicit reverse_iterator(iterator_type x) : current_(x) {}
    
    template <typename U>
    reverse_iterator(const reverse_iterator<U>& other) : current_(other.base()) {}

    // 获取基础迭代器
    iterator_type base() const { return current_; }

    // 解引用
    reference operator*() const {
        iterator_type tmp = current_;
        --tmp;
        return *tmp;
    }

    pointer operator->() const {
        return std::addressof(operator*());
    }

    // 前置递增
    reverse_iterator& operator++() {
        --current_;
        return *this;
    }

    // 后置递增
    reverse_iterator operator++(int) {
        reverse_iterator tmp = *this;
        --current_;
        return tmp;
    }

    // 前置递减
    reverse_iterator& operator--() {
        ++current_;
        return *this;
    }

    // 后置递减
    reverse_iterator operator--(int) {
        reverse_iterator tmp = *this;
        ++current_;
        return tmp;
    }

    // 算术运算
    reverse_iterator operator+(difference_type n) const {
        return reverse_iterator(current_ - n);
    }

    reverse_iterator operator-(difference_type n) const {
        return reverse_iterator(current_ + n);
    }

    reverse_iterator& operator+=(difference_type n) {
        current_ -= n;
        return *this;
    }

    reverse_iterator& operator-=(difference_type n) {
        current_ += n;
        return *this;
    }

    reference operator[](difference_type n) const {
        return *(*this + n);
    }

private:
    iterator_type current_;
};

// 反向迭代器比较操作
template <typename Iterator>
bool operator==(const reverse_iterator<Iterator>& x, const reverse_iterator<Iterator>& y) {
    return x.base() == y.base();
}

template <typename Iterator>
bool operator!=(const reverse_iterator<Iterator>& x, const reverse_iterator<Iterator>& y) {
    return x.base() != y.base();
}

template <typename Iterator>
bool operator<(const reverse_iterator<Iterator>& x, const reverse_iterator<Iterator>& y) {
    return x.base() > y.base();
}

template <typename Iterator>
bool operator<=(const reverse_iterator<Iterator>& x, const reverse_iterator<Iterator>& y) {
    return x.base() >= y.base();
}

template <typename Iterator>
bool operator>(const reverse_iterator<Iterator>& x, const reverse_iterator<Iterator>& y) {
    return x.base() < y.base();
}

template <typename Iterator>
bool operator>=(const reverse_iterator<Iterator>& x, const reverse_iterator<Iterator>& y) {
    return x.base() <= y.base();
}

template <typename Iterator>
auto operator-(const reverse_iterator<Iterator>& x, const reverse_iterator<Iterator>& y) {
    return y.base() - x.base();
}

template <typename Iterator>
auto operator+(typename reverse_iterator<Iterator>::difference_type n, 
               const reverse_iterator<Iterator>& x) {
    return reverse_iterator<Iterator>(x.base() - n);
}

// 插入迭代器基类
template <typename Container>
class insert_iterator {
public:
    using iterator_category = output_iterator_tag;
    using value_type = void;
    using difference_type = void;
    using pointer = void;
    using reference = void;
    using container_type = Container;

    insert_iterator(Container& c, typename Container::iterator i)
        : container_(std::addressof(c)), iter_(i) {}

    insert_iterator& operator=(const typename Container::value_type& value) {
        iter_ = container_->insert(iter_, value);
        ++iter_;
        return *this;
    }

    insert_iterator& operator=(typename Container::value_type&& value) {
        iter_ = container_->insert(iter_, std::move(value));
        ++iter_;
        return *this;
    }

    insert_iterator& operator*() { return *this; }
    insert_iterator& operator++() { return *this; }
    insert_iterator& operator++(int) { return *this; }

protected:
    Container* container_;
    typename Container::iterator iter_;
};

// 创建插入迭代器的辅助函数
template <typename Container>
insert_iterator<Container> inserter(Container& c, typename Container::iterator i) {
    return insert_iterator<Container>(c, i);
}

// 前向插入迭代器
template <typename Container>
class front_insert_iterator {
public:
    using iterator_category = output_iterator_tag;
    using value_type = void;
    using difference_type = void;
    using pointer = void;
    using reference = void;
    using container_type = Container;

    explicit front_insert_iterator(Container& c) : container_(std::addressof(c)) {}

    front_insert_iterator& operator=(const typename Container::value_type& value) {
        container_->push_front(value);
        return *this;
    }

    front_insert_iterator& operator=(typename Container::value_type&& value) {
        container_->push_front(std::move(value));
        return *this;
    }

    front_insert_iterator& operator*() { return *this; }
    front_insert_iterator& operator++() { return *this; }
    front_insert_iterator& operator++(int) { return *this; }

protected:
    Container* container_;
};

// 创建前向插入迭代器的辅助函数
template <typename Container>
front_insert_iterator<Container> front_inserter(Container& c) {
    return front_insert_iterator<Container>(c);
}

// 后向插入迭代器
template <typename Container>
class back_insert_iterator {
public:
    using iterator_category = output_iterator_tag;
    using value_type = void;
    using difference_type = void;
    using pointer = void;
    using reference = void;
    using container_type = Container;

    explicit back_insert_iterator(Container& c) : container_(std::addressof(c)) {}

    back_insert_iterator& operator=(const typename Container::value_type& value) {
        container_->push_back(value);
        return *this;
    }

    back_insert_iterator& operator=(typename Container::value_type&& value) {
        container_->push_back(std::move(value));
        return *this;
    }

    back_insert_iterator& operator*() { return *this; }
    back_insert_iterator& operator++() { return *this; }
    back_insert_iterator& operator++(int) { return *this; }

protected:
    Container* container_;
};

// 创建后向插入迭代器的辅助函数
template <typename Container>
back_insert_iterator<Container> back_inserter(Container& c) {
    return back_insert_iterator<Container>(c);
}

// 移动迭代器
template <typename Iterator>
class move_iterator {
public:
    using iterator_type = Iterator;
    using iterator_category = typename iterator_traits<Iterator>::iterator_category;
    using value_type = typename iterator_traits<Iterator>::value_type;
    using difference_type = typename iterator_traits<Iterator>::difference_type;
    using pointer = Iterator;
    using reference = value_type&&;

    move_iterator() : current_() {}
    
    explicit move_iterator(iterator_type i) : current_(i) {}
    
    template <typename U>
    move_iterator(const move_iterator<U>& u) : current_(u.base()) {}

    iterator_type base() const { return current_; }

    reference operator*() const {
        return std::move(*current_);
    }

    pointer operator->() const {
        return current_;
    }

    move_iterator& operator++() {
        ++current_;
        return *this;
    }

    move_iterator operator++(int) {
        move_iterator tmp = *this;
        ++current_;
        return tmp;
    }

    move_iterator& operator--() {
        --current_;
        return *this;
    }

    move_iterator operator--(int) {
        move_iterator tmp = *this;
        --current_;
        return tmp;
    }

    move_iterator operator+(difference_type n) const {
        return move_iterator(current_ + n);
    }

    move_iterator operator-(difference_type n) const {
        return move_iterator(current_ - n);
    }

    move_iterator& operator+=(difference_type n) {
        current_ += n;
        return *this;
    }

    move_iterator& operator-=(difference_type n) {
        current_ -= n;
        return *this;
    }

    reference operator[](difference_type n) const {
        return std::move(current_[n]);
    }

private:
    iterator_type current_;
};

// 移动迭代器比较操作
template <typename Iterator>
bool operator==(const move_iterator<Iterator>& x, const move_iterator<Iterator>& y) {
    return x.base() == y.base();
}

template <typename Iterator>
bool operator!=(const move_iterator<Iterator>& x, const move_iterator<Iterator>& y) {
    return x.base() != y.base();
}

template <typename Iterator>
bool operator<(const move_iterator<Iterator>& x, const move_iterator<Iterator>& y) {
    return x.base() < y.base();
}

template <typename Iterator>
bool operator<=(const move_iterator<Iterator>& x, const move_iterator<Iterator>& y) {
    return x.base() <= y.base();
}

template <typename Iterator>
bool operator>(const move_iterator<Iterator>& x, const move_iterator<Iterator>& y) {
    return x.base() > y.base();
}

template <typename Iterator>
bool operator>=(const move_iterator<Iterator>& x, const move_iterator<Iterator>& y) {
    return x.base() >= y.base();
}

template <typename Iterator>
auto operator-(const move_iterator<Iterator>& x, const move_iterator<Iterator>& y) {
    return x.base() - y.base();
}

template <typename Iterator>
auto operator+(typename move_iterator<Iterator>::difference_type n, 
               const move_iterator<Iterator>& x) {
    return move_iterator<Iterator>(x.base() + n);
}

template <typename Iterator>
auto operator+(const move_iterator<Iterator>& x, 
               typename move_iterator<Iterator>::difference_type n) {
    return move_iterator<Iterator>(x.base() + n);
}

// 创建移动迭代器的辅助函数
template <typename Iterator>
move_iterator<Iterator> make_move_iterator(Iterator i) {
    return move_iterator<Iterator>(i);
}

// 迭代器距离计算
template <typename InputIterator>
typename iterator_traits<InputIterator>::difference_type
distance(InputIterator first, InputIterator last) {
    using category = typename iterator_traits<InputIterator>::iterator_category;
    
    if constexpr (std::is_base_of_v<random_access_iterator_tag, category>) {
        return last - first;
    } else {
        typename iterator_traits<InputIterator>::difference_type result = 0;
        while (first != last) {
            ++first;
            ++result;
        }
        return result;
    }
}

// 迭代器前进
template <typename InputIterator, typename Distance>
void advance(InputIterator& i, Distance n) {
    using category = typename iterator_traits<InputIterator>::iterator_category;
    
    if constexpr (std::is_base_of_v<std::random_access_iterator_tag, category> || 
                  std::is_base_of_v<random_access_iterator_tag, category>) {
        i += n;
    } else if constexpr (std::is_base_of_v<std::bidirectional_iterator_tag, category> || 
                        std::is_base_of_v<bidirectional_iterator_tag, category>) {
        if (n > 0) {
            while (n--) ++i;
        } else {
            n = -n;
            while (n--) --i;
        }
    } else {
        if (n < 0) {
            // 对于只能前进的迭代器，不能负向移动
            return;
        }
        while (n--) ++i;
    }
}

// 迭代器下一个位置
template <typename InputIterator>
InputIterator next(InputIterator x, 
                  typename iterator_traits<InputIterator>::difference_type n = 1) {
    InputIterator result = x;
    stl::advance(result, n);
    return result;
}

// 迭代器前一个位置
template <typename BidirectionalIterator>
BidirectionalIterator prev(BidirectionalIterator x,
                         typename iterator_traits<BidirectionalIterator>::difference_type n = 1) {
    BidirectionalIterator result = x;
    stl::advance(result, -n);
    return result;
}

// 开始和结束迭代器
template <typename Container>
auto begin(Container& c) -> decltype(c.begin()) {
    return c.begin();
}

template <typename Container>
auto begin(const Container& c) -> decltype(c.begin()) {
    return c.begin();
}

template <typename Container>
auto end(Container& c) -> decltype(c.end()) {
    return c.end();
}

template <typename Container>
auto end(const Container& c) -> decltype(c.end()) {
    return c.end();
}

template <typename T, std::size_t N>
T* begin(T (&array)[N]) {
    return array;
}

template <typename T, std::size_t N>
T* end(T (&array)[N]) {
    return array + N;
}

// C++14 cbegin/cend
template <typename Container>
auto cbegin(const Container& c) -> decltype(c.begin()) {
    return c.begin();
}

template <typename Container>
auto cend(const Container& c) -> decltype(c.end()) {
    return c.end();
}

// rbegin/rend
template <typename Container>
auto rbegin(Container& c) -> decltype(c.rbegin()) {
    return c.rbegin();
}

template <typename Container>
auto rbegin(const Container& c) -> decltype(c.rbegin()) {
    return c.rbegin();
}

template <typename Container>
auto rend(Container& c) -> decltype(c.rend()) {
    return c.rend();
}

template <typename Container>
auto rend(const Container& c) -> decltype(c.rend()) {
    return c.rend();
}

template <typename T, std::size_t N>
reverse_iterator<T*> rbegin(T (&array)[N]) {
    return reverse_iterator<T*>(array + N);
}

template <typename T, std::size_t N>
reverse_iterator<T*> rend(T (&array)[N]) {
    return reverse_iterator<T*>(array);
}

// crbegin/crend
template <typename Container>
auto crbegin(const Container& c) -> decltype(c.rbegin()) {
    return c.rbegin();
}

template <typename Container>
auto crend(const Container& c) -> decltype(c.rend()) {
    return c.rend();
}

} // namespace stl

#endif // STL_ITERATOR_HPP
#include <gtest/gtest.h>
#include <vector>
#include <list>
#include <forward_list>
#include <string>

// 使用命名空间避免冲突
#include "stl/iterator.hpp"

namespace stl {

// 测试迭代器特性萃取器
TEST(IteratorTest, IteratorTraits) {
    // 测试指针类型的特性萃取
    int* ptr = nullptr;
    using ptr_traits = iterator_traits<decltype(ptr)>;
    
    static_assert(std::is_same_v<ptr_traits::iterator_category, random_access_iterator_tag>);
    static_assert(std::is_same_v<ptr_traits::value_type, int>);
    static_assert(std::is_same_v<ptr_traits::difference_type, std::ptrdiff_t>);
    static_assert(std::is_same_v<ptr_traits::pointer, int*>);
    static_assert(std::is_same_v<ptr_traits::reference, int&>);
    
    // 测试const指针类型的特性萃取
    const int* const_ptr = nullptr;
    using const_ptr_traits = iterator_traits<decltype(const_ptr)>;
    
    static_assert(std::is_same_v<const_ptr_traits::iterator_category, random_access_iterator_tag>);
    static_assert(std::is_same_v<const_ptr_traits::value_type, int>);
    static_assert(std::is_same_v<const_ptr_traits::difference_type, std::ptrdiff_t>);
    static_assert(std::is_same_v<const_ptr_traits::pointer, const int*>);
    static_assert(std::is_same_v<const_ptr_traits::reference, const int&>);
}

// 测试反向迭代器
TEST(IteratorTest, ReverseIterator) {
    std::vector<int> vec = {1, 2, 3, 4, 5};
    
    reverse_iterator<std::vector<int>::iterator> rbegin(vec.end());
    reverse_iterator<std::vector<int>::iterator> rend(vec.begin());
    
    // 测试解引用
    EXPECT_EQ(*rbegin, 5);
    // 注意：rend指向begin之前，解引用是未定义行为
    // 我们不测试*rend
    
    // 测试递增
    ++rbegin;
    EXPECT_EQ(*rbegin, 4);
    
    // 测试递减
    --rbegin;
    EXPECT_EQ(*rbegin, 5);
    
    // 测试算术运算
    EXPECT_EQ(*(rbegin + 1), 4);
    // rbegin - 1 moves past the end, which is undefined behavior
    // We'll test the position instead
    auto rbegin_minus = rbegin - 1;
    EXPECT_EQ(rbegin_minus.base() - vec.begin(), 6); // Past the end
    
    // 测试比较运算
    EXPECT_TRUE(rbegin == rbegin);
    EXPECT_TRUE(rbegin != rend);
    // 注意：反向迭代器的比较逻辑与正向相反
    // rbegin指向vec.end()（元素5），rend指向vec.begin()（元素1之前）
    // 在反向序列中，rbegin在rend之前，所以rbegin < rend
    EXPECT_TRUE(rbegin < rend);
    EXPECT_TRUE(rend > rbegin);
}

// 测试反向迭代器遍历
TEST(IteratorTest, ReverseIteratorTraversal) {
    std::vector<int> vec = {1, 2, 3, 4, 5};
    std::vector<int> reversed;
    
    reverse_iterator<std::vector<int>::iterator> rbegin(vec.end());
    reverse_iterator<std::vector<int>::iterator> rend(vec.begin());
    
    for (auto it = rbegin; it != rend; ++it) {
        reversed.push_back(*it);
    }
    
    std::vector<int> expected = {5, 4, 3, 2, 1};
    EXPECT_EQ(reversed, expected);
}

// 测试插入迭代器
TEST(IteratorTest, InsertIterator) {
    std::vector<int> vec = {1, 2, 5};
    auto it = vec.begin() + 2;
    
    auto inserter = stl::inserter(vec, it);
    *inserter = 3;
    *inserter = 4;
    
    std::vector<int> expected = {1, 2, 3, 4, 5};
    EXPECT_EQ(vec, expected);
}

// 测试前向插入迭代器
TEST(IteratorTest, FrontInsertIterator) {
    std::list<int> lst = {3, 4, 5};
    
    auto front_inserter = stl::front_inserter(lst);
    *front_inserter = 2;
    *front_inserter = 1;
    
    std::list<int> expected = {1, 2, 3, 4, 5};
    EXPECT_EQ(lst, expected);
}

// 测试后向插入迭代器
TEST(IteratorTest, BackInsertIterator) {
    std::vector<int> vec = {1, 2, 3};
    
    auto back_inserter = stl::back_inserter(vec);
    *back_inserter = 4;
    *back_inserter = 5;
    
    std::vector<int> expected = {1, 2, 3, 4, 5};
    EXPECT_EQ(vec, expected);
}

// 测试移动迭代器
TEST(IteratorTest, MoveIterator) {
    std::vector<std::string> vec = {"hello", "world", "test"};
    std::vector<std::string> moved;
    
    move_iterator<std::vector<std::string>::iterator> move_begin(vec.begin());
    move_iterator<std::vector<std::string>::iterator> move_end(vec.end());
    
    for (auto it = move_begin; it != move_end; ++it) {
        moved.push_back(*it);
    }
    
    // 检查移动后的内容
    EXPECT_EQ(moved.size(), 3);
    EXPECT_EQ(moved[0], "hello");
    EXPECT_EQ(moved[1], "world");
    EXPECT_EQ(moved[2], "test");
    
    // 检查原容器中的字符串是否为空（如果移动语义正确实现）
    // 注意：std::string的移动实现可能保留内容，所以这里不做严格检查
}

// 测试移动迭代器与标准算法
TEST(IteratorTest, MoveIteratorWithAlgorithm) {
    std::vector<std::string> src = {"one", "two", "three"};
    std::vector<std::string> dest;
    
    auto move_begin = stl::make_move_iterator(src.begin());
    auto move_end = stl::make_move_iterator(src.end());
    
    for (auto it = move_begin; it != move_end; ++it) {
        dest.push_back(*it);
    }
    
    EXPECT_EQ(dest.size(), 3);
    EXPECT_EQ(dest[0], "one");
    EXPECT_EQ(dest[1], "two");
    EXPECT_EQ(dest[2], "three");
}

// 测试迭代器距离计算
TEST(IteratorTest, Distance) {
    std::vector<int> vec = {1, 2, 3, 4, 5};
    
    // 随机访问迭代器
    auto dist = stl::distance(vec.begin(), vec.end());
    EXPECT_EQ(dist, 5);
    
    dist = stl::distance(vec.begin() + 1, vec.end() - 1);
    EXPECT_EQ(dist, 3);
    
    // 前向迭代器（使用list）
    std::list<int> lst = {1, 2, 3, 4, 5};
    dist = stl::distance(lst.begin(), lst.end());
    EXPECT_EQ(dist, 5);
}

// 测试迭代器前进
TEST(IteratorTest, Advance) {
    std::vector<int> vec = {1, 2, 3, 4, 5};
    auto it = vec.begin();
    
    // 正向前进
    stl::advance(it, 3);
    EXPECT_EQ(*it, 4);
    
    // 负向前进（随机访问迭代器）
    stl::advance(it, -2);
    EXPECT_EQ(*it, 2);
    
    // 测试前向迭代器
    std::list<int> lst = {1, 2, 3, 4, 5};
    auto lit = lst.begin();
    stl::advance(lit, 3);
    EXPECT_EQ(*lit, 4);
}

// 测试next和prev
TEST(IteratorTest, NextPrev) {
    std::vector<int> vec = {1, 2, 3, 4, 5};
    auto it = vec.begin() + 2; // 指向3
    
    // 测试next
    auto next_it = stl::next(it);
    EXPECT_EQ(*next_it, 4);
    
    next_it = stl::next(it, 2);
    EXPECT_EQ(*next_it, 5);
    
    // 测试prev
    auto prev_it = stl::prev(it);
    EXPECT_EQ(*prev_it, 2);
    
    prev_it = stl::prev(it, 2);
    EXPECT_EQ(*prev_it, 1);
}

// 测试begin/end函数
TEST(IteratorTest, BeginEnd) {
    std::vector<int> vec = {1, 2, 3, 4, 5};
    
    // 测试容器
    auto b = stl::begin(vec);
    auto e = stl::end(vec);
    EXPECT_EQ(*b, 1);
    EXPECT_EQ(*(e - 1), 5);
    
    // 测试const容器
    const std::vector<int>& cvec = vec;
    auto cb = stl::begin(cvec);
    auto ce = stl::end(cvec);
    EXPECT_EQ(*cb, 1);
    EXPECT_EQ(*(ce - 1), 5);
    
    // 测试数组
    int arr[] = {1, 2, 3, 4, 5};
    auto ab = stl::begin(arr);
    auto ae = stl::end(arr);
    EXPECT_EQ(*ab, 1);
    EXPECT_EQ(*(ae - 1), 5);
}

// 测试cbegin/cend
TEST(IteratorTest, CBeginCEnd) {
    std::vector<int> vec = {1, 2, 3, 4, 5};
    
    auto cb = stl::cbegin(vec);
    auto ce = stl::cend(vec);
    EXPECT_EQ(*cb, 1);
    EXPECT_EQ(*(ce - 1), 5);
    
    // 确保返回的是const迭代器
    static_assert(std::is_same_v<decltype(cb), std::vector<int>::const_iterator>);
}

// 测试rbegin/rend
TEST(IteratorTest, RBeginREnd) {
    std::vector<int> vec = {1, 2, 3, 4, 5};
    
    auto rb = stl::rbegin(vec);
    auto re = stl::rend(vec);
    EXPECT_EQ(*rb, 5);
    EXPECT_EQ(*(--re), 1);
    
    // 测试数组
    int arr[] = {1, 2, 3, 4, 5};
    auto arr_rb = stl::rbegin(arr);
    auto arr_re = stl::rend(arr);
    EXPECT_EQ(*arr_rb, 5);
    EXPECT_EQ(*(--arr_re), 1);
}

// 测试crbegin/crend
TEST(IteratorTest, CRBeginCREnd) {
    std::vector<int> vec = {1, 2, 3, 4, 5};
    
    auto crb = stl::crbegin(vec);
    auto cre = stl::crend(vec);
    EXPECT_EQ(*crb, 5);
    EXPECT_EQ(*(--cre), 1);
    
    // 确保返回的是const反向迭代器
    static_assert(std::is_same_v<decltype(crb), std::vector<int>::const_reverse_iterator>);
}

// 测试迭代器算术运算
TEST(IteratorTest, IteratorArithmetic) {
    std::vector<int> vec = {1, 2, 3, 4, 5};
    
    // 测试反向迭代器算术运算
    reverse_iterator<std::vector<int>::iterator> rit(vec.end());
    
    // 加法
    auto rit_plus = rit + 2;
    EXPECT_EQ(*rit_plus, 3);
    
    // 减法 - rit - 1 moves past the end, which is undefined behavior
    // We'll test the position instead
    auto rit_minus = rit - 1;
    EXPECT_EQ(rit_minus.base() - vec.begin(), 6); // Past the end
    
    // 复合赋值
    rit += 2;
    EXPECT_EQ(*rit, 3);
    
    rit -= 1;
    EXPECT_EQ(*rit, 4);
}

// 测试迭代器比较运算
TEST(IteratorTest, IteratorComparison) {
    std::vector<int> vec = {1, 2, 3, 4, 5};
    
    reverse_iterator<std::vector<int>::iterator> r1(vec.end());
    reverse_iterator<std::vector<int>::iterator> r2(vec.end());
    reverse_iterator<std::vector<int>::iterator> r3(vec.begin());
    
    EXPECT_EQ(r1, r2);
    EXPECT_NE(r1, r3);
    EXPECT_LT(r1, r3);
    EXPECT_GT(r3, r1);
    EXPECT_LE(r1, r2);
    EXPECT_GE(r3, r1);
}

// 测试迭代器与标准库兼容性
TEST(IteratorTest, StandardLibraryCompatibility) {
    std::vector<int> vec = {1, 2, 3, 4, 5};
    
    // 使用我们的反向迭代器与标准算法
    reverse_iterator<std::vector<int>::iterator> rbegin(vec.end());
    reverse_iterator<std::vector<int>::iterator> rend(vec.begin());
    
    std::vector<int> reversed;
    std::copy(rbegin, rend, std::back_inserter(reversed));
    
    std::vector<int> expected = {5, 4, 3, 2, 1};
    EXPECT_EQ(reversed, expected);
}

} // namespace stl
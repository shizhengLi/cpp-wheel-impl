#include <gtest/gtest.h>
#include <vector>
#include <string>
#include "stl/algorithm.hpp"

class AlgorithmTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup test data
        small_vec = {5, 2, 8, 1, 9, 3};
        sorted_vec = {1, 2, 3, 4, 5, 6};
        string_vec = {"apple", "banana", "cherry", "date"};
    }

    std::vector<int> small_vec;
    std::vector<int> sorted_vec;
    std::vector<std::string> string_vec;
};

// 基本算法测试
TEST_F(AlgorithmTest, BasicAlgorithms) {
    // copy测试
    std::vector<int> dest(6, 0);
    auto result = ::stl::copy(small_vec.begin(), small_vec.end(), dest.begin());
    EXPECT_EQ(dest.size(), small_vec.size());
    for (size_t i = 0; i < dest.size(); ++i) {
        EXPECT_EQ(dest[i], small_vec[i]);
    }
    EXPECT_EQ(result, dest.end());
    
    // fill测试
    std::vector<int> fill_vec(5, 0);
    ::stl::fill(fill_vec.begin(), fill_vec.end(), 42);
    for (size_t i = 0; i < fill_vec.size(); ++i) {
        EXPECT_EQ(fill_vec[i], 42);
    }
    
    // fill_n测试
    std::vector<int> fill_n_vec(5, 0);
    auto fill_result = ::stl::fill_n(fill_n_vec.begin(), 3, 99);
    EXPECT_EQ(fill_n_vec[0], 99);
    EXPECT_EQ(fill_n_vec[1], 99);
    EXPECT_EQ(fill_n_vec[2], 99);
    EXPECT_EQ(fill_n_vec[3], 0);
    EXPECT_EQ(fill_result, fill_n_vec.begin() + 3);
    
    // swap测试
    int a = 10, b = 20;
    ::stl::swap(a, b);
    EXPECT_EQ(a, 20);
    EXPECT_EQ(b, 10);
    
    // swap_ranges测试
    std::vector<int> vec1 = {1, 2, 3};
    std::vector<int> vec2 = {4, 5, 6};
    auto swap_result = ::stl::swap_ranges(vec1.begin(), vec1.end(), vec2.begin());
    EXPECT_EQ(vec1[0], 4);
    EXPECT_EQ(vec1[1], 5);
    EXPECT_EQ(vec1[2], 6);
    EXPECT_EQ(vec2[0], 1);
    EXPECT_EQ(vec2[1], 2);
    EXPECT_EQ(vec2[2], 3);
    EXPECT_EQ(swap_result, vec2.begin() + 3);
}

// Min/Max算法测试
TEST_F(AlgorithmTest, MinMaxAlgorithms) {
    // min测试
    EXPECT_EQ(::stl::min(5, 10), 5);
    EXPECT_EQ(::stl::min(10, 5), 5);
    EXPECT_EQ(::stl::min('a', 'b'), 'a');
    
    // max测试
    EXPECT_EQ(::stl::max(5, 10), 10);
    EXPECT_EQ(::stl::max(10, 5), 10);
    EXPECT_EQ(::stl::max('a', 'b'), 'b');
    
    // minmax测试
    auto minmax_int = ::stl::minmax(5, 10);
    EXPECT_EQ(minmax_int.first, 5);
    EXPECT_EQ(minmax_int.second, 10);
    
    auto minmax_char = ::stl::minmax('b', 'a');
    EXPECT_EQ(minmax_char.first, 'a');
    EXPECT_EQ(minmax_char.second, 'b');
    
    // min_element测试
    auto min_it = ::stl::min_element(small_vec.begin(), small_vec.end());
    EXPECT_EQ(*min_it, 1);
    
    // max_element测试
    auto max_it = ::stl::max_element(small_vec.begin(), small_vec.end());
    EXPECT_EQ(*max_it, 9);
    
    // minmax_element测试
    auto minmax_result = ::stl::minmax_element(small_vec.begin(), small_vec.end());
    EXPECT_EQ(*minmax_result.first, 1);
    EXPECT_EQ(*minmax_result.second, 9);
}

// 查找算法测试
TEST_F(AlgorithmTest, SearchAlgorithms) {
    // find测试
    auto find_result = ::stl::find(small_vec.begin(), small_vec.end(), 8);
    EXPECT_NE(find_result, small_vec.end());
    EXPECT_EQ(*find_result, 8);
    
    auto not_found = ::stl::find(small_vec.begin(), small_vec.end(), 99);
    EXPECT_EQ(not_found, small_vec.end());
    
    // find_if测试
    auto find_if_result = ::stl::find_if(small_vec.begin(), small_vec.end(), [](int x) { return x > 5; });
    EXPECT_NE(find_if_result, small_vec.end());
    EXPECT_EQ(*find_if_result, 8);
    
    // find_if_not测试
    auto find_if_not_result = ::stl::find_if_not(small_vec.begin(), small_vec.end(), [](int x) { return x < 5; });
    EXPECT_NE(find_if_not_result, small_vec.end());
    EXPECT_EQ(*find_if_not_result, 5);
    
    // count测试
    std::vector<int> count_vec = {1, 2, 2, 3, 2, 4};
    auto count_result = ::stl::count(count_vec.begin(), count_vec.end(), 2);
    EXPECT_EQ(count_result, 3);
    
    // count_if测试
    auto count_if_result = ::stl::count_if(count_vec.begin(), count_vec.end(), [](int x) { return x % 2 == 0; });
    EXPECT_EQ(count_if_result, 4);
    
    // adjacent_find测试
    std::vector<int> adj_vec = {1, 2, 2, 3, 4, 4, 5};
    auto adj_result = ::stl::adjacent_find(adj_vec.begin(), adj_vec.end());
    EXPECT_NE(adj_result, adj_vec.end());
    EXPECT_EQ(*adj_result, 2);
    
    // equal测试
    std::vector<int> equal_vec1 = {1, 2, 3};
    std::vector<int> equal_vec2 = {1, 2, 3};
    std::vector<int> unequal_vec = {1, 2, 4};
    
    EXPECT_TRUE(::stl::equal(equal_vec1.begin(), equal_vec1.end(), equal_vec2.begin()));
    EXPECT_FALSE(::stl::equal(equal_vec1.begin(), equal_vec1.end(), unequal_vec.begin()));
}

// 二分查找算法测试
TEST_F(AlgorithmTest, BinarySearchAlgorithms) {
    // lower_bound测试
    auto lb_result = ::stl::lower_bound(sorted_vec.begin(), sorted_vec.end(), 4);
    EXPECT_NE(lb_result, sorted_vec.end());
    EXPECT_EQ(*lb_result, 4);
    
    auto lb_result2 = ::stl::lower_bound(sorted_vec.begin(), sorted_vec.end(), 5);
    EXPECT_NE(lb_result2, sorted_vec.end());
    EXPECT_EQ(*lb_result2, 5);
    
    // upper_bound测试
    auto ub_result = ::stl::upper_bound(sorted_vec.begin(), sorted_vec.end(), 3);
    EXPECT_NE(ub_result, sorted_vec.end());
    EXPECT_EQ(*ub_result, 4);
    
    // binary_search测试
    EXPECT_TRUE(::stl::binary_search(sorted_vec.begin(), sorted_vec.end(), 3));
    EXPECT_TRUE(::stl::binary_search(sorted_vec.begin(), sorted_vec.end(), 6));
    EXPECT_FALSE(::stl::binary_search(sorted_vec.begin(), sorted_vec.end(), 0));
    EXPECT_FALSE(::stl::binary_search(sorted_vec.begin(), sorted_vec.end(), 7));
    
    // equal_range测试
    std::vector<int> dup_vec = {1, 2, 2, 2, 3, 4};
    auto range_result = ::stl::equal_range(dup_vec.begin(), dup_vec.end(), 2);
    EXPECT_EQ(*range_result.first, 2);
    EXPECT_EQ(*range_result.second, 3);
    auto dist = 0;
    for (auto it = range_result.first; it != range_result.second; ++it) {
        ++dist;
    }
    EXPECT_EQ(dist, 3);
}

// 排序算法测试
TEST_F(AlgorithmTest, SortAlgorithms) {
    // sort测试
    std::vector<int> sort_vec = {5, 2, 8, 1, 9, 3};
    ::stl::sort(sort_vec.begin(), sort_vec.end());
    EXPECT_TRUE(::stl::is_sorted(sort_vec.begin(), sort_vec.end()));
    std::vector<int> expected = {1, 2, 3, 5, 8, 9};
    EXPECT_EQ(sort_vec.size(), expected.size());
    for (size_t i = 0; i < sort_vec.size(); ++i) {
        EXPECT_EQ(sort_vec[i], expected[i]);
    }
    
    // stable_sort测试
    std::vector<int> stable_vec = {5, 2, 8, 1, 9, 3};
    ::stl::stable_sort(stable_vec.begin(), stable_vec.end());
    EXPECT_TRUE(::stl::is_sorted(stable_vec.begin(), stable_vec.end()));
    
    // partial_sort测试
    std::vector<int> partial_vec = {5, 2, 8, 1, 9, 3};
    ::stl::partial_sort(partial_vec.begin(), partial_vec.begin() + 3, partial_vec.end());
    EXPECT_EQ(partial_vec[0], 1);
    EXPECT_EQ(partial_vec[1], 2);
    EXPECT_EQ(partial_vec[2], 3);
    
    // partition测试
    std::vector<int> part_vec = {1, 2, 3, 4, 5, 6};
    auto part_result = ::stl::partition(part_vec.begin(), part_vec.end(), [](int x) { return x % 2 == 0; });
    EXPECT_NE(part_result, part_vec.end());
    for (auto it = part_vec.begin(); it != part_result; ++it) {
        EXPECT_EQ(*it % 2, 0);
    }
    for (auto it = part_result; it != part_vec.end(); ++it) {
        EXPECT_EQ(*it % 2, 1);
    }
    
    // nth_element测试
    std::vector<int> nth_vec = {5, 2, 8, 1, 9, 3};
    ::stl::nth_element(nth_vec.begin(), nth_vec.begin() + 2, nth_vec.end());
    EXPECT_EQ(nth_vec[2], 3);
    for (auto it = nth_vec.begin(); it != nth_vec.begin() + 2; ++it) {
        EXPECT_LE(*it, 3);
    }
    for (auto it = nth_vec.begin() + 3; it != nth_vec.end(); ++it) {
        EXPECT_GE(*it, 3);
    }
    
    // is_sorted测试
    EXPECT_TRUE(::stl::is_sorted(sorted_vec.begin(), sorted_vec.end()));
    EXPECT_FALSE(::stl::is_sorted(small_vec.begin(), small_vec.end()));
    
    // is_sorted_until测试
    std::vector<int> partial_sorted = {1, 2, 3, 5, 4, 6};
    auto sorted_until = ::stl::is_sorted_until(partial_sorted.begin(), partial_sorted.end());
    EXPECT_NE(sorted_until, partial_sorted.end());
    EXPECT_EQ(*sorted_until, 4);
    auto dist = 0;
    for (auto it = partial_sorted.begin(); it != sorted_until; ++it) {
        ++dist;
    }
    EXPECT_EQ(dist, 4);
}

// 堆算法测试
TEST_F(AlgorithmTest, HeapAlgorithms) {
    // make_heap测试
    std::vector<int> heap_vec = {1, 2, 3, 4, 5, 6};
    ::stl::make_heap(heap_vec.begin(), heap_vec.end());
    EXPECT_TRUE(::stl::is_heap(heap_vec.begin(), heap_vec.end()));
    EXPECT_EQ(heap_vec[0], 6); // 最大元素应该在堆顶
    
    // push_heap测试
    heap_vec.push_back(7);
    ::stl::push_heap(heap_vec.begin(), heap_vec.end());
    EXPECT_TRUE(::stl::is_heap(heap_vec.begin(), heap_vec.end()));
    EXPECT_EQ(heap_vec[0], 7);
    
    // pop_heap测试
    ::stl::pop_heap(heap_vec.begin(), heap_vec.end());
    EXPECT_EQ(heap_vec.back(), 7);
    heap_vec.pop_back();
    EXPECT_TRUE(::stl::is_heap(heap_vec.begin(), heap_vec.end()));
    
    // sort_heap测试
    std::vector<int> sort_heap_vec = {3, 1, 4, 1, 5, 9};
    ::stl::make_heap(sort_heap_vec.begin(), sort_heap_vec.end());
    ::stl::sort_heap(sort_heap_vec.begin(), sort_heap_vec.end());
    EXPECT_TRUE(::stl::is_sorted(sort_heap_vec.begin(), sort_heap_vec.end()));
    
    // is_heap测试
    std::vector<int> not_heap = {1, 2, 3, 4, 5};
    EXPECT_FALSE(::stl::is_heap(not_heap.begin(), not_heap.end()));
    
    std::vector<int> is_heap_vec = {5, 4, 3, 2, 1};
    EXPECT_TRUE(::stl::is_heap(is_heap_vec.begin(), is_heap_vec.end()));
    
    // is_heap_until测试
    std::vector<int> partial_heap = {5, 4, 3, 2, 6, 1};
    auto heap_until = ::stl::is_heap_until(partial_heap.begin(), partial_heap.end());
    EXPECT_NE(heap_until, partial_heap.end());
    EXPECT_EQ(*heap_until, 6);
    auto dist = 0;
    for (auto it = partial_heap.begin(); it != heap_until; ++it) {
        ++dist;
    }
    EXPECT_EQ(dist, 4);
}

// 字典序比较测试
TEST_F(AlgorithmTest, LexicographicalCompare) {
    std::vector<int> vec1 = {1, 2, 3};
    std::vector<int> vec2 = {1, 2, 4};
    std::vector<int> vec3 = {1, 2, 3};
    std::vector<int> vec4 = {1, 2, 3, 4};
    
    EXPECT_TRUE(::stl::lexicographical_compare(vec1.begin(), vec1.end(), vec2.begin(), vec2.end()));
    EXPECT_FALSE(::stl::lexicographical_compare(vec2.begin(), vec2.end(), vec1.begin(), vec1.end()));
    EXPECT_FALSE(::stl::lexicographical_compare(vec1.begin(), vec1.end(), vec3.begin(), vec3.end()));
    EXPECT_TRUE(::stl::lexicographical_compare(vec1.begin(), vec1.end(), vec4.begin(), vec4.end()));
}

// 搜索算法测试
TEST_F(AlgorithmTest, SearchPatternAlgorithms) {
    // search测试
    std::vector<int> text = {1, 2, 3, 4, 5, 6, 7, 8};
    std::vector<int> pattern = {4, 5, 6};
    
    auto search_result = ::stl::search(text.begin(), text.end(), pattern.begin(), pattern.end());
    EXPECT_NE(search_result, text.end());
    EXPECT_EQ(*search_result, 4);
    
    std::vector<int> not_found_pattern = {9, 10};
    auto not_found = ::stl::search(text.begin(), text.end(), not_found_pattern.begin(), not_found_pattern.end());
    EXPECT_EQ(not_found, text.end());
    
    // search_n测试
    std::vector<int> search_n_vec = {1, 2, 2, 2, 3, 4};
    auto search_n_result = ::stl::search_n(search_n_vec.begin(), search_n_vec.end(), 3, 2);
    EXPECT_NE(search_n_result, search_n_vec.end());
    EXPECT_EQ(*search_n_result, 2);
    
    auto search_n_not_found = ::stl::search_n(search_n_vec.begin(), search_n_vec.end(), 2, 5);
    EXPECT_EQ(search_n_not_found, search_n_vec.end());
    
    // find_first_of测试
    std::vector<int> find_first_vec = {1, 2, 3, 4, 5};
    std::vector<int> first_of_pattern = {3, 7, 9};
    
    auto first_of_result = ::stl::find_first_of(find_first_vec.begin(), find_first_vec.end(), 
                                              first_of_pattern.begin(), first_of_pattern.end());
    EXPECT_NE(first_of_result, find_first_vec.end());
    EXPECT_EQ(*first_of_result, 3);
    
    // find_end测试
    std::vector<int> find_end_text = {1, 2, 3, 1, 2, 3, 4};
    std::vector<int> find_end_pattern = {1, 2, 3};
    
    auto find_end_result = ::stl::find_end(find_end_text.begin(), find_end_text.end(), 
                                       find_end_pattern.begin(), find_end_pattern.end());
    EXPECT_NE(find_end_result, find_end_text.end());
    EXPECT_EQ(*find_end_result, 1);
    auto dist = 0;
    for (auto it = find_end_text.begin(); it != find_end_result; ++it) {
        ++dist;
    }
    EXPECT_EQ(dist, 3);
}
#include <gtest/gtest.h>
#include <memory>
#include <type_traits>
#include <thread>
#include <mutex>
#include <atomic>

#include "../include/shared_ptr.hpp"

// 测试基本构造和析构
TEST(SharedPtrTest, BasicConstruction) {
    my::shared_ptr<int> p1;
    EXPECT_EQ(p1.get(), nullptr);
    EXPECT_EQ(p1.use_count(), 0);
    
    my::shared_ptr<int> p2(new int(42));
    EXPECT_NE(p2.get(), nullptr);
    EXPECT_EQ(*p2, 42);
    EXPECT_EQ(p2.use_count(), 1);
}

// 测试拷贝构造
TEST(SharedPtrTest, CopyConstruction) {
    my::shared_ptr<int> p1(new int(42));
    my::shared_ptr<int> p2(p1);
    
    EXPECT_EQ(p1.get(), p2.get());
    EXPECT_EQ(*p1, *p2);
    EXPECT_EQ(p1.use_count(), 2);
    EXPECT_EQ(p2.use_count(), 2);
}

// 测试拷贝赋值
TEST(SharedPtrTest, CopyAssignment) {
    my::shared_ptr<int> p1(new int(42));
    my::shared_ptr<int> p2;
    
    p2 = p1;
    EXPECT_EQ(p1.get(), p2.get());
    EXPECT_EQ(*p1, *p2);
    EXPECT_EQ(p1.use_count(), 2);
    EXPECT_EQ(p2.use_count(), 2);
}

// 测试移动构造
TEST(SharedPtrTest, MoveConstruction) {
    my::shared_ptr<int> p1(new int(42));
    int* raw_ptr = p1.get();
    
    my::shared_ptr<int> p2(std::move(p1));
    
    EXPECT_EQ(p2.get(), raw_ptr);
    EXPECT_EQ(*p2, 42);
    EXPECT_EQ(p2.use_count(), 1);
    EXPECT_EQ(p1.get(), nullptr);
    EXPECT_EQ(p1.use_count(), 0);
}

// 测试移动赋值
TEST(SharedPtrTest, MoveAssignment) {
    my::shared_ptr<int> p1(new int(42));
    my::shared_ptr<int> p2(new int(100));
    int* raw_ptr = p1.get();
    
    p2 = std::move(p1);
    
    EXPECT_EQ(p2.get(), raw_ptr);
    EXPECT_EQ(*p2, 42);
    EXPECT_EQ(p2.use_count(), 1);
    EXPECT_EQ(p1.get(), nullptr);
    EXPECT_EQ(p1.use_count(), 0);
}

// 测试reset方法
TEST(SharedPtrTest, ResetMethod) {
    my::shared_ptr<int> p1(new int(42));
    my::shared_ptr<int> p2(p1);
    
    EXPECT_EQ(p1.use_count(), 2);
    
    p1.reset();
    EXPECT_EQ(p1.get(), nullptr);
    EXPECT_EQ(p1.use_count(), 0);
    EXPECT_EQ(p2.use_count(), 1);
    
    p2.reset(new int(100));
    EXPECT_EQ(*p2, 100);
    EXPECT_EQ(p2.use_count(), 1);
}

// 测试引用计数
TEST(SharedPtrTest, ReferenceCounting) {
    my::shared_ptr<int> p1(new int(42));
    EXPECT_EQ(p1.use_count(), 1);
    
    {
        my::shared_ptr<int> p2(p1);
        EXPECT_EQ(p1.use_count(), 2);
        EXPECT_EQ(p2.use_count(), 2);
        
        {
            my::shared_ptr<int> p3(p2);
            EXPECT_EQ(p1.use_count(), 3);
            EXPECT_EQ(p2.use_count(), 3);
            EXPECT_EQ(p3.use_count(), 3);
        }
        
        EXPECT_EQ(p1.use_count(), 2);
        EXPECT_EQ(p2.use_count(), 2);
    }
    
    EXPECT_EQ(p1.use_count(), 1);
}

// 测试自定义删除器
TEST(SharedPtrTest, CustomDeleter) {
    bool deleted = false;
    auto deleter = [&deleted](int* p) {
        deleted = true;
        delete p;
    };
    
    {
        my::shared_ptr<int> p(new int(42), deleter);
        EXPECT_EQ(*p, 42);
        EXPECT_FALSE(deleted);
    }
    
    EXPECT_TRUE(deleted);
}

// 测试make_shared
TEST(SharedPtrTest, MakeShared) {
    my::shared_ptr<int> p = my::make_shared<int>(42);
    EXPECT_EQ(*p, 42);
    EXPECT_EQ(p.use_count(), 1);
    
    my::shared_ptr<std::string> s = my::make_shared<std::string>("Hello");
    EXPECT_EQ(*s, "Hello");
    EXPECT_EQ(s.use_count(), 1);
}

// 测试操作符重载
TEST(SharedPtrTest, OperatorOverloading) {
    my::shared_ptr<int> p(new int(42));
    
    // 解引用操作符
    EXPECT_EQ(*p, 42);
    *p = 100;
    EXPECT_EQ(*p, 100);
    
    // 箭头操作符
    my::shared_ptr<std::string> s = my::make_shared<std::string>("Hello");
    EXPECT_EQ(s->size(), 5);
    s->append(" World");
    EXPECT_EQ(*s, "Hello World");
    
    // 布尔操作符
    my::shared_ptr<int> empty;
    EXPECT_FALSE(empty);
    EXPECT_TRUE(p);
}

// 测试比较操作符
TEST(SharedPtrTest, ComparisonOperators) {
    my::shared_ptr<int> p1(new int(42));
    my::shared_ptr<int> p2(p1);
    my::shared_ptr<int> p3(new int(42));
    my::shared_ptr<int> p4;
    
    EXPECT_EQ(p1, p2);
    EXPECT_NE(p1, p3);
    EXPECT_NE(p1, p4);
    
    // 指针地址比较（取决于内存分配顺序）
    if (p1.get() < p3.get()) {
        EXPECT_LT(p1, p3);
        EXPECT_GT(p3, p1);
    } else {
        EXPECT_GT(p1, p3);
        EXPECT_LT(p3, p1);
    }
    EXPECT_LE(p1, p2);
    EXPECT_GE(p2, p1);
}

// 测试所有权转移
TEST(SharedPtrTest, OwnershipTransfer) {
    my::shared_ptr<int> p1(new int(42));
    my::shared_ptr<int> p2;
    
    EXPECT_EQ(p1.use_count(), 1);
    EXPECT_EQ(p2.use_count(), 0);
    
    p2 = p1;
    EXPECT_EQ(p1.use_count(), 2);
    EXPECT_EQ(p2.use_count(), 2);
    
    p1.reset();
    EXPECT_EQ(p1.use_count(), 0);
    EXPECT_EQ(p2.use_count(), 1);
}

// 测试异常安全
TEST(SharedPtrTest, ExceptionSafety) {
    my::shared_ptr<int> p1(new int(42));
    
    // 测试空指针访问
    my::shared_ptr<int> p2;
    EXPECT_THROW(*p2, std::runtime_error);
}

// 测试类型特征
TEST(SharedPtrTest, TypeTraits) {
    EXPECT_TRUE(std::is_copy_constructible_v<my::shared_ptr<int>>);
    EXPECT_TRUE(std::is_copy_assignable_v<my::shared_ptr<int>>);
    EXPECT_TRUE(std::is_move_constructible_v<my::shared_ptr<int>>);
    EXPECT_TRUE(std::is_move_assignable_v<my::shared_ptr<int>>);
}

// 测试复杂类型
TEST(SharedPtrTest, ComplexTypes) {
    my::shared_ptr<std::string> s = my::make_shared<std::string>("Hello");
    EXPECT_EQ(*s, "Hello");
    EXPECT_EQ(s->size(), 5);
    
    s->append(" World");
    EXPECT_EQ(*s, "Hello World");
    EXPECT_EQ(s->size(), 11);
}

// 测试数组类型
TEST(SharedPtrTest, ArrayTypes) {
    auto deleter = [](int* p) { delete[] p; };
    my::shared_ptr<int> arr(new int[5], deleter);
    
    for (int i = 0; i < 5; ++i) {
        arr.get()[i] = i;
    }
    
    for (int i = 0; i < 5; ++i) {
        EXPECT_EQ(arr.get()[i], i);
    }
}

// 测试多线程安全性
TEST(SharedPtrTest, ThreadSafety) {
    my::shared_ptr<int> p = my::make_shared<int>(42);
    std::vector<std::thread> threads;
    std::atomic<int> counter{0};
    
    // 创建多个线程来测试引用计数的线程安全性
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back([&p, &counter]() {
            for (int j = 0; j < 1000; ++j) {
                my::shared_ptr<int> local = p;
                counter.fetch_add(1, std::memory_order_relaxed);
            }
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    EXPECT_EQ(counter.load(), 10000);
    EXPECT_EQ(p.use_count(), 1);
}

// 测试循环引用
TEST(SharedPtrTest, CircularReference) {
    struct Node {
        my::shared_ptr<Node> next;
        int value;
        Node(int v) : value(v) {}
    };
    
    my::shared_ptr<Node> node1 = my::make_shared<Node>(1);
    my::shared_ptr<Node> node2 = my::make_shared<Node>(2);
    
    // 创建循环引用
    node1->next = node2;
    node2->next = node1;
    
    EXPECT_EQ(node1.use_count(), 2);
    EXPECT_EQ(node2.use_count(), 2);
    
    // 手动断开循环引用
    node1->next.reset();
    node2->next.reset();
    
    EXPECT_EQ(node1.use_count(), 1);
    EXPECT_EQ(node2.use_count(), 1);
}

// 测试weak_ptr支持（如果实现了）
TEST(SharedPtrTest, WeakPtrSupport) {
    my::shared_ptr<int> p = my::make_shared<int>(42);
    
    // 测试weak_ptr构造
    my::weak_ptr<int> w(p);
    EXPECT_EQ(w.use_count(), 1);
    EXPECT_FALSE(w.expired());
    
    // 测试lock方法
    my::shared_ptr<int> p2 = w.lock();
    EXPECT_EQ(*p2, 42);
    EXPECT_EQ(p2.use_count(), 2);
    
    // p2离开作用域后，引用计数应该恢复
    p2.reset();
    EXPECT_EQ(w.use_count(), 1);
    
    // 测试expired
    p.reset();
    EXPECT_TRUE(w.expired());
    EXPECT_EQ(w.lock().get(), nullptr);
}

// 测试内存管理
TEST(SharedPtrTest, MemoryManagement) {
    std::atomic<int> delete_count{0};
    
    auto deleter = [&delete_count](int* p) {
        delete_count.fetch_add(1);
        delete p;
    };
    
    {
        my::shared_ptr<int> p1(new int(42), deleter);
        my::shared_ptr<int> p2(p1);
        my::shared_ptr<int> p3(p2);
        
        EXPECT_EQ(delete_count.load(), 0);
    }
    
    EXPECT_EQ(delete_count.load(), 1);
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
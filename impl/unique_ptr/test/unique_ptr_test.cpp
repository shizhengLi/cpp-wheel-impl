#include <gtest/gtest.h>
#include <memory>
#include <type_traits>

#include "unique_ptr.hpp"

// 测试类，用于验证析构函数调用
class TestClass {
public:
    TestClass(int value = 0) : value_(value) {
        std::cout << "TestClass constructed with value " << value_ << std::endl;
    }
    
    ~TestClass() {
        std::cout << "TestClass destroyed with value " << value_ << std::endl;
    }
    
    int getValue() const { return value_; }
    void setValue(int value) { value_ = value; }
    
private:
    int value_;
};

// 测试基本构造和析构
TEST(UniquePtrTest, BasicConstruction) {
    {
        my::unique_ptr<TestClass> ptr(new TestClass(42));
        EXPECT_EQ(ptr->getValue(), 42);
        EXPECT_EQ((*ptr).getValue(), 42);
    }
    // ptr离开作用域，应该自动调用析构函数
}

// 测试移动构造
TEST(UniquePtrTest, MoveConstruction) {
    my::unique_ptr<TestClass> ptr1(new TestClass(100));
    EXPECT_EQ(ptr1->getValue(), 100);
    
    my::unique_ptr<TestClass> ptr2 = std::move(ptr1);
    EXPECT_EQ(ptr2->getValue(), 100);
    EXPECT_EQ(ptr1.get(), nullptr);  // ptr1应该为空
}

// 测试移动赋值
TEST(UniquePtrTest, MoveAssignment) {
    my::unique_ptr<TestClass> ptr1(new TestClass(200));
    my::unique_ptr<TestClass> ptr2(new TestClass(300));
    
    ptr2 = std::move(ptr1);
    EXPECT_EQ(ptr2->getValue(), 200);
    EXPECT_EQ(ptr1.get(), nullptr);  // ptr1应该为空
}

// 测试拷贝构造应该被删除
TEST(UniquePtrTest, CopyConstructionShouldBeDeleted) {
    my::unique_ptr<TestClass> ptr1(new TestClass(400));
    
    // 以下代码应该编译失败
    // my::unique_ptr<TestClass> ptr2 = ptr1;
}

// 测试拷贝赋值应该被删除
TEST(UniquePtrTest, CopyAssignmentShouldBeDeleted) {
    my::unique_ptr<TestClass> ptr1(new TestClass(500));
    my::unique_ptr<TestClass> ptr2(new TestClass(600));
    
    // 以下代码应该编译失败
    // ptr2 = ptr1;
}

// 测试reset方法
TEST(UniquePtrTest, ResetMethod) {
    my::unique_ptr<TestClass> ptr(new TestClass(700));
    EXPECT_EQ(ptr->getValue(), 700);
    
    ptr.reset(new TestClass(800));
    EXPECT_EQ(ptr->getValue(), 800);
    
    ptr.reset();
    EXPECT_EQ(ptr.get(), nullptr);
}

// 测试release方法
TEST(UniquePtrTest, ReleaseMethod) {
    TestClass* raw_ptr = new TestClass(900);
    my::unique_ptr<TestClass> ptr(raw_ptr);
    EXPECT_EQ(ptr->getValue(), 900);
    
    TestClass* released_ptr = ptr.release();
    EXPECT_EQ(released_ptr, raw_ptr);
    EXPECT_EQ(ptr.get(), nullptr);
    
    // 需要手动释放，因为unique_ptr不再管理
    delete released_ptr;
}

// 测试get方法
TEST(UniquePtrTest, GetMethod) {
    TestClass* raw_ptr = new TestClass(1000);
    my::unique_ptr<TestClass> ptr(raw_ptr);
    
    EXPECT_EQ(ptr.get(), raw_ptr);
    EXPECT_EQ(ptr.get()->getValue(), 1000);
}

// 测试operator bool
TEST(UniquePtrTest, OperatorBool) {
    my::unique_ptr<TestClass> ptr1(new TestClass(1100));
    my::unique_ptr<TestClass> ptr2;
    
    EXPECT_TRUE(ptr1);
    EXPECT_FALSE(ptr2);
}

// 测试swap方法
TEST(UniquePtrTest, SwapMethod) {
    my::unique_ptr<TestClass> ptr1(new TestClass(1200));
    my::unique_ptr<TestClass> ptr2(new TestClass(1300));
    
    ptr1.swap(ptr2);
    
    EXPECT_EQ(ptr1->getValue(), 1300);
    EXPECT_EQ(ptr2->getValue(), 1200);
}

// 测试自定义删除器
template<typename T>
struct CustomDeleter {
    void operator()(T* ptr) const {
        std::cout << "Custom deleter called for value " << ptr->getValue() << std::endl;
        delete ptr;
    }
};

TEST(UniquePtrTest, CustomDeleter) {
    my::unique_ptr<TestClass, CustomDeleter<TestClass>> ptr(new TestClass(1400));
    EXPECT_EQ(ptr->getValue(), 1400);
}

// 测试数组特化
TEST(UniquePtrTest, ArraySpecialization) {
    my::unique_ptr<int[]> ptr(new int[5]);
    
    for (int i = 0; i < 5; ++i) {
        ptr[i] = i * 10;
    }
    
    for (int i = 0; i < 5; ++i) {
        EXPECT_EQ(ptr[i], i * 10);
    }
}

// 测试make_unique工厂函数
TEST(UniquePtrTest, MakeUnique) {
    auto ptr = my::make_unique<TestClass>(1500);
    EXPECT_EQ(ptr->getValue(), 1500);
    
    auto ptr2 = my::make_unique<int[]>(5);
    for (int i = 0; i < 5; ++i) {
        ptr2[i] = i * 20;
    }
    for (int i = 0; i < 5; ++i) {
        EXPECT_EQ(ptr2[i], i * 20);
    }
}

// 测试空指针操作
TEST(UniquePtrTest, NullPointerOperations) {
    my::unique_ptr<TestClass> ptr;
    
    EXPECT_FALSE(ptr);
    EXPECT_EQ(ptr.get(), nullptr);
    
    // 测试对空指针的operator*应该抛出异常
    EXPECT_THROW(*ptr, std::runtime_error);
    
    // 测试对空指针的operator->应该抛出异常
    EXPECT_THROW(ptr->getValue(), std::runtime_error);
}

// 测试类型特征
TEST(UniquePtrTest, TypeTraits) {
    EXPECT_TRUE(std::is_move_constructible_v<my::unique_ptr<int>>);
    EXPECT_TRUE(std::is_move_assignable_v<my::unique_ptr<int>>);
    EXPECT_FALSE(std::is_copy_constructible_v<my::unique_ptr<int>>);
    EXPECT_FALSE(std::is_copy_assignable_v<my::unique_ptr<int>>);
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
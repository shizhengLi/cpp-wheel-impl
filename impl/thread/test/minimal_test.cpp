#include <gtest/gtest.h>
#include <atomic>

#include "../include/thread.hpp"

TEST(ThreadTest, MinimalDetach) {
    std::atomic<bool> flag{false};
    
    my::thread t([&flag]() {
        flag = true;
    });
    
    EXPECT_TRUE(t.joinable());
    t.detach();
    EXPECT_FALSE(t.joinable());
    
    // 等待线程完成
    for (int i = 0; i < 100; ++i) {
        if (flag) break;
        usleep(1000);
    }
    EXPECT_TRUE(flag);
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
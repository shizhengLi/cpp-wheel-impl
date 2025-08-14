字节跳动C++二面：手写shared_ptr实现
图片
在 C++ 开发领域，智能指针是一项极为重要的机制，它极大地提升了内存管理的安全性与便捷性。其中，shared_ptr作为智能指针家族的关键成员，采用引用计数的方式，允许多个指针共享同一对象的所有权，当最后一个指向对象的shared_ptr被销毁时，对象所占用的内存会被自动释放，有效规避了内存泄漏的风险。

在字节跳动 C++ 二面中，要求手写shared_ptr实现，这一题目旨在深度考察面试者对 C++ 内存管理、对象生命周期把控、模板编程以及运算符重载等多方面核心知识的理解与运用能力。要想成功完成这一挑战，面试者不仅需要清晰掌握shared_ptr的底层运行原理，还得能够用严谨、高效且符合 C++ 最佳实践的代码将其准确呈现出来。接下来，我们就一同深入剖析如何逐步实现一个简易版的shared_ptr 。


Part1什么是shared_ptr？
在 C++ 编程的世界中，内存管理一直是一个让人又爱又恨的话题。手动管理内存就像是在走钢丝，稍有不慎就会引发内存泄漏、悬空指针等一系列严重的问题，让程序变得脆弱不堪。为了解决这些棘手的问题，C++11 引入了智能指针这个强大的工具，而std::shared_ptr就是其中的佼佼者。不清楚指针的，可以看看这篇《探索C++内存管理：指针、引用和内存分配》

std:shared_ptr是 C++ 标准库提供的一种智能指针，它采用了引用计数的机制，允许多个指针共享同一个对象的所有权 ，这就好比多个小伙伴共同拥有一个玩具，每个人都对这个玩具有一定的使用权。当最后一个拥有这个玩具的小伙伴不再需要它时（即引用计数为 0），玩具就会被自动回收，这样就避免了手动管理内存时可能出现的各种麻烦。

例如，在一个图形渲染引擎中，有多个模块可能需要访问同一个纹理资源。使用std::shared_ptr，可以让这些模块共享对纹理资源的引用，而不必担心资源的释放问题。当所有模块都不再使用该纹理时，std::shared_ptr会自动释放纹理占用的内存，大大提高了内存管理的安全性和效率。

在实际使用中，std::shared_ptr的操作非常灵活。比如，我们可以通过std::make_shared函数来创建一个std::shared_ptr对象，这种方式不仅更加简洁，而且效率更高，因为它一次性分配了对象和引用计数所需的内存。同时，std::shared_ptr还支持赋值、比较等操作，使得代码的编写更加自然和直观。

1.1产生的原因
shared_ptr的产生与unique_ptr类似，都是为了解决raw pointer的new和delete的成对使用，导致的野指针、内存泄漏、重复释放内存等。

不过shared_ptr与unique_ptr场景又有所不同，这里主要是一个raw pointer在不同的代码块之间传来传去的场景，或者指针指向的内存比较大，这段内存可以切分成很多小部分，但是他们却需要共享彼此的数据。

1.2 shared_ptr特性
shared_ptr 有两个特性：

特性1: 对raw pointer进行了一层封装，让C++程序员不用再担心何时去释放分配好的内存。
特性2: 共享，使用shared_ptr的指针可以共享同一块内存中的数据。
思想是：该类型智能指针在实现上采用的是引用计数机制，即便有一个 shared_ptr 指针放弃了堆内存的“使用权”（引用计数减 1），也不会影响其他指向同一堆内存的 shared_ptr 指针（只有引用计数为 0 时，堆内存才会被自动释放）。

1.3 shared_ptr的优势
（1）自动释放：当最后一个std::shared_ptr离开作用域时，引用计数变为零，它会自动调用对象的析构函数，防止内存泄漏 。在一个网络通信模块中，我们可能会创建一个std::shared_ptr来管理一个连接对象。当所有与该连接相关的操作完成，并且指向该连接对象的std::shared_ptr都超出作用域时，连接对象会被自动释放，无需手动调用析构函数。这样一来，我们就不用担心因为疏忽而导致连接资源没有被正确释放，从而大大提高了代码的可靠性。

#include <iostream>
#include <memory>

// 模拟网络连接对象
class Connection {
public:
    Connection() { std::cout << "Connection established\n"; }
    ~Connection() { std::cout << "Connection released\n"; }

    void send(const std::string& data) {
        std::cout << "Sending data: " << data << "\n";
    }
};

// 模拟网络通信模块
void handleConnection() {
    // 创建 shared_ptr 管理连接对象（引用计数=1）
    auto conn = std::make_shared<Connection>();

    // 模拟多个操作共享该连接
    auto task1 = [conn]() { conn->send("Task1 data"); };
    auto task2 = [conn]() { conn->send("Task2 data"); }; // 引用计数增加到3

    task1();
    task2();

    // task1/task2的lambda析构，引用计数减回1
} // conn离开作用域，引用计数归零，自动调用Connection析构

int main() {
    handleConnection();

    // 输出示例：
    // Connection established
    // Sending data: Task1 data
    // Sending data: Task2 data
    // Connection released

    return 0;
}
（2）对象共享：多个std::shared_ptr可以指向同一对象，这使得资源共享的实现变得更加简单。在一个游戏开发项目中，多个游戏对象可能需要共享同一个纹理资源。通过std::shared_ptr，我们可以轻松地让这些游戏对象共享对纹理的引用，而不必为每个对象单独复制纹理数据，节省了内存空间，同时也提高了资源的利用率。

#include <iostream>
#include <memory>
#include <vector>

// 模拟纹理资源类
class Texture {
public:
    Texture(const std::string& path) : m_path(path) {
        std::cout << "Loaded texture: " << m_path << "\n";
    }
    ~Texture() { std::cout << "Unloaded texture: " << m_path << "\n"; }

    void render(int x, int y) const {
        std::cout << "Rendering texture at (" << x << ", " << y 
                  << ") from: " << m_path << "\n";
    }

private:
    std::string m_path;
};

// 游戏对象基类（使用共享纹理）
class GameObject {
public:
    GameObject(std::shared_ptr<Texture> texture) : m_texture(texture) {}

    virtual void draw() = 0;

protected:
    std::shared_ptr<Texture> m_texture; // 共享纹理的智能指针
};

// 具体游戏对象：角色
class Character : public GameObject {
public:
    using GameObject::GameObject;

    void draw() override {
        m_texture->render(100, 200); // 使用共享纹理
        std::cout << "Character drawn\n";
    }
};

// 具体游戏对象：场景背景
class Background : public GameObject {
public:
    using GameObject::GameObject;

    void draw() override {
        m_texture->render(0, 0); // 使用同一份纹理（可能缩放或裁剪）
        std::cout << "Background drawn\n";
    }
};

int main() {
    // 1. 加载纹理（仅一次）
    auto sharedTexture = std::make_shared<Texture>("assets/hero.png");

    // 2. 创建多个共享该纹理的游戏对象
    Character hero(sharedTexture);
    Background scene(sharedTexture);

    // 3. 渲染时无需关心纹理生命周期
    hero.draw();
    scene.draw();

    // 4. main结束时，所有对象析构后引用计数归零，自动释放纹理

   return 0;
}
（3）异常安全：std::shared_ptr的引用计数会自动管理，不会因为函数异常退出而泄漏内存。在进行复杂的数据库操作时，可能会涉及多个步骤，其中任何一步都有可能抛出异常。如果我们使用std::shared_ptr来管理数据库连接对象，即使在操作过程中发生异常，std::shared_ptr也会确保连接对象被正确释放，避免了内存泄漏和资源悬空的问题，保证了程序的健壮性。


#include <iostream>
#include <memory>
#include <stdexcept>

// 模拟数据库连接类
class DatabaseConnection {
public:
    DatabaseConnection(const std::string& url) : m_url(url) {
        std::cout << "Connected to database: " << m_url << "\n";
    }

    ~DatabaseConnection() {
        std::cout << "Disconnected from database: " << m_url << "\n";
    }

    void executeQuery(const std::string& sql) {
        if (sql.empty()) throw std::runtime_error("Empty SQL query");
        std::cout << "Executed: " << sql << "\n";
    }

private:
    std::string m_url;
};

// 高风险操作：可能抛出异常的复杂数据库事务
void riskyDatabaseOperation(std::shared_ptr<DatabaseConnection> conn) {
    conn->executeQuery("BEGIN TRANSACTION");  // 步骤1：开始事务

    // 模拟可能失败的操作（如违反约束）
    conn->executeQuery("UPDATE users SET balance = -100 WHERE id = 123"); // 步骤2：可能抛异常

    conn->executeQuery("COMMIT");             // 步骤3：提交事务（若异常则不会执行）
}

int main() {
    try {
        // 1. 创建共享的数据库连接
        auto dbConn = std::make_shared<DatabaseConnection>("mysql://localhost:3306");

        // 2. 执行高风险操作（即使内部抛出异常，conn也会被释放）
        riskyDatabaseOperation(dbConn);

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";

        // 无需手动释放dbConn！shared_ptr会在栈展开时自动析构
    }

   return 0;
}
Part2shared_ptr的使用方法
2.1创建 shared_ptr 对象
在 C++ 中，创建std::shared_ptr对象主要有两种常见方式。一种是使用std::make_shared函数 ，它能一次性分配对象和控制块的内存，效率较高，语法也更为简洁。比如创建一个指向int类型对象的std::shared_ptr，可以这样写：

auto ptr1 = std::make_shared<int>(42);
这就创建了一个std::shared_ptr，它指向一个值为 42 的int对象。

另一种方式是使用原始指针来构造std::shared_ptr，但这种方式需要注意原始指针必须是通过new分配的，否则会导致未定义行为 。示例如下：

int* rawPtr = new int(10);
std::shared_ptr<int> ptr2(rawPtr);
这里先创建了一个原始指针rawPtr，然后用它构造了std::shared_ptr对象ptr2。不过这种方式相对繁琐，且容易出错，因为需要手动管理原始指针的生命周期，所以更推荐使用std::make_shared。

2.2引用计数相关操作
std::shared_ptr的核心特性之一就是引用计数，通过一些成员函数，我们可以对引用计数进行操作和查询。use_count函数用于返回当前有多少个std::shared_ptr指向同一个对象，主要用于调试目的 。例如：

auto ptr3 = std::make_shared<int>(100);
std::cout << "ptr3的引用计数: " << ptr3.use_count() << std::endl; 
auto ptr4 = ptr3;
std::cout << "赋值后ptr3的引用计数: " << ptr3.use_count() << std::endl; 
在上述代码中，首先创建ptr3时，其引用计数为 1；当把ptr3赋值给ptr4后，它们指向同一个对象，引用计数变为 2。

unique函数则用于判断当前std::shared_ptr是否是指向对象的唯一指针 ，如果是则返回true，否则返回false。接着上面的代码：

if (ptr3.unique()) {
    std::cout << "ptr3是唯一指向对象的指针" << std::endl;
} else {
    std::cout << "ptr3不是唯一指向对象的指针" << std::endl;
}
由于ptr3和ptr4共享对象，所以ptr3.unique()会返回false。

reset函数用于重置std::shared_ptr，它有两种常见用法。不带参数调用reset时，会减少引用计数，如果引用计数变为 0，就会释放所指向的对象，并将当前std::shared_ptr置为空 。例如：

ptr3.reset();
std::cout << "reset后ptr3的引用计数: " << ptr3.use_count() << std::endl; 
此时ptr3的引用计数变为 0（如果没有其他指向该对象的std::shared_ptr），对象被释放，ptr3变为空指针。

带参数调用reset时，会先减少原对象的引用计数，然后让std::shared_ptr指向新的对象 。例如：

ptr4.reset(new int(200));
std::cout << "ptr4指向新对象后的引用计数: " << ptr4.use_count() << std::endl; 
这里ptr4先减少对原对象的引用计数，然后指向一个新的值为 200 的int对象，引用计数变为 1。

2.3作为普通指针使用
std::shared_ptr重载了*和->操作符，这使得它可以像普通指针一样使用 。通过*操作符可以解引用std::shared_ptr，访问其所指向的对象的值。例如：

auto ptr5 = std::make_shared<int>(50);
int value = *ptr5;
std::cout << "ptr5指向的值: " << value << std::endl; 
这里通过*ptr5获取到ptr5指向的int对象的值，并赋值给value。

而->操作符则用于访问所指向对象的成员函数或成员变量。假设有一个自定义类MyClass：

class MyClass {
public:
    void print() {
        std::cout << "这是MyClass的成员函数" << std::endl;
    }
};
auto ptr6 = std::make_shared<MyClass>();
ptr6->print(); 
在这段代码中，通过ptr6->print()调用了MyClass对象的print成员函数，就如同使用普通指针一样方便 。这种重载操作符的设计，使得std::shared_ptr在使用上更加直观和自然，无需额外的函数调用就能完成对对象的操作。

Part3循环引用问题及解决
3.1循环引用示例
虽然std::shared_ptr在内存管理方面表现出色，但在使用过程中，如果不注意其引用计数机制，就可能会遇到循环引用的问题 。循环引用是指两个或多个对象相互引用对方的std::shared_ptr，从而导致引用计数永远无法归零，最终造成内存泄漏。

下面是一个具体的代码示例，展示了循环引用是如何发生的：

#include <iostream>
#include <memory>

class B;

class A {
public:
    std::shared_ptr<B> b_ptr;
    ~A() {
        std::cout << "A destroyed" << std::endl;
    }
};

class B {
public:
    std::shared_ptr<A> a_ptr;
    ~B() {
        std::cout << "B destroyed" << std::endl;
    }
};

int main() {
    auto a = std::make_shared<A>();
    auto b = std::make_shared<B>();

    a->b_ptr = b;
    b->a_ptr = a;

    return 0;
} 
在上述代码中，A类和B类相互持有对方的std::shared_ptr。当main函数结束时，a和b的局部变量被销毁，它们的引用计数会减 1，但由于a的b_ptr指向b，b的a_ptr指向a，所以a和b的引用计数始终无法降为 0 ，导致A和B的析构函数都不会被调用，内存无法释放，从而发生内存泄漏。这种循环引用的问题在实际项目中可能很难被发现，尤其是在复杂的对象关系中，它会逐渐消耗系统资源，影响程序的性能和稳定性 。

3.2使用 std::weak_ptr 打破循环引用
为了解决std::shared_ptr的循环引用问题，C++ 引入了std::weak_ptr 。std::weak_ptr是一种弱引用智能指针，它不拥有所指向对象的所有权，不会增加对象的引用计数，主要用于解决std::shared_ptr循环引用的问题，同时提供一种安全的方式来访问std::shared_ptr所管理的对象 。


std::weak_ptr的核心特性之一是它的lock方法，该方法用于尝试获取一个指向所引用对象的std::shared_ptr 。如果对象已经被释放，lock方法会返回一个空的std::shared_ptr，通过这种方式可以安全地检查对象是否仍然有效，避免悬空指针的问题。

下面是使用std::weak_ptr解决上述循环引用问题的代码示例：

#include <iostream>
#include <memory>

class B;

class A {
public:
    std::shared_ptr<B> b_ptr;
    ~A() {
        std::cout << "A destroyed" << std::endl;
    }
};

class B {
public:
    std::weak_ptr<A> a_weak;
    ~B() {
        std::cout << "B destroyed" << std::endl;
    }
};

int main() {
    auto a = std::make_shared<A>();
    auto b = std::make_shared<B>();

    a->b_ptr = b;
    b->a_weak = a;

    return 0;
} 
在这个改进后的代码中，B类不再持有A类的std::shared_ptr，而是使用std::weak_ptr来引用A 。当main函数结束时，a的引用计数会因为局部变量a的销毁而减为 0，A对象被正确释放；接着，b的引用计数也会减为 0，B对象也被释放，从而避免了循环引用导致的内存泄漏问题 。

当需要通过std::weak_ptr访问对象时，可以使用lock方法，例如：

class B {
public:
    std::weak_ptr<A> a_weak;
    void accessA() {
        auto temp = a_weak.lock();
        if (temp) {
            // 安全地访问A对象
            temp->someFunction(); 
        } else {
            std::cout << "A对象已被释放" << std::endl;
        }
    }
    ~B() {
        std::cout << "B destroyed" << std::endl;
    }
}; 
在上述代码的accessA方法中，先通过lock方法获取A对象的std::shared_ptr，并检查其是否为空 。如果不为空，就可以安全地访问A对象的成员；如果为空，说明A对象已经被释放，避免了悬空指针的风险。

Part4shared_ptr的线程安全性
4.1引用计数的线程安全
在多线程环境下，std::shared_ptr的引用计数操作是线程安全的，这是它的一个重要特性 。当多个线程同时对同一个std::shared_ptr进行复制、赋值或销毁等操作时，其引用计数的递增和递减是通过原子操作来实现的，无需额外的加锁机制 。这就好比有一个公共的计数器，多个线程可以同时对它进行增加或减少操作，而且不会出现计数错误的情况。

例如，在一个多线程的文件处理系统中，多个线程可能同时读取或处理同一个文件对象，每个线程都持有一个std::shared_ptr指向该文件对象 。当某个线程完成对文件的处理，其持有的std::shared_ptr离开作用域时，引用计数会自动减 1，这个过程是线程安全的，不会因为多线程并发操作而导致引用计数错误，从而保证了文件对象在所有线程都不再需要时能被正确释放 。这种原子操作的实现，使得std::shared_ptr在多线程环境下的引用计数管理变得高效且可靠，大大降低了因多线程操作导致的内存管理错误风险 。

4.2访问对象的线程安全
虽然std::shared_ptr的引用计数是线程安全的，但对其指向对象的访问却并非如此 。当多个线程同时通过std::shared_ptr访问和修改同一个对象时，如果没有适当的同步措施，就可能会引发数据竞争和未定义行为 。例如，在一个多线程的银行账户管理系统中，多个线程可能同时对同一个账户对象进行取款和存款操作 。如果直接通过std::shared_ptr访问账户对象，而不进行任何同步控制，就可能出现数据不一致的情况，比如一个线程读取了账户余额后，还未进行更新操作，另一个线程又读取了相同的余额并进行操作，最终导致账户余额计算错误 。

为了保证对std::shared_ptr指向对象的安全访问，通常需要使用诸如std::mutex（互斥锁）、std::lock_guard（RAII 风格的锁管理类）等同步机制 。下面是一个使用std::mutex来保护共享对象访问的代码示例：

#include <iostream>
#include <memory>
#include <mutex>
#include <thread>

class Counter {
public:
    Counter() : value(0) {}
    void increment() {
        std::lock_guard<std::mutex> lock(mtx);
        ++value;
    }
    int get() {
        std::lock_guard<std::mutex> lock(mtx);
        return value;
    }
private:
    int value;
    std::mutex mtx;
};

int main() {
    auto counter = std::make_shared<Counter>();
    std::thread threads[10];
    for (int i = 0; i < 10; ++i) {
        threads[i] = std::thread([counter]() {
            for (int j = 0; j < 100; ++j) {
                counter->increment();
            }
        });
    }
    for (auto& th : threads) {
        th.join();
    }
    std::cout << "Final counter value: " << counter->get() << std::endl; 
    return 0;
} 
在上述代码中，Counter类包含一个std::mutex成员变量mtx，用于保护对value成员变量的访问 。在increment和get成员函数中，使用std::lock_guard<std::mutex>来自动管理锁的生命周期，在进入函数时自动加锁，离开函数时自动解锁 。这样，当多个线程同时调用increment函数时，通过锁机制保证了每次只有一个线程能够修改value，从而避免了数据竞争，确保了线程安全 。

注意事项：虽然std::shared_ptr确保了引用计数的线程安全，但对对象本身的访问并非线程安全。如果多个线程要修改std::shared_ptr指向的对象，仍然需要额外的同步措施（如使用std::mutex）来保证线程安全。

4.3多线程修改 std::shared_ptr 指向的对象
如果多个线程需要同时访问并修改 std::shared_ptr 指向的对象，使用 std::mutex 可以保证线程安全。这里提供一个示例展示如何使用 std::mutex 来保护对共享对象的访问和修改。

我们创建一个共享的计数器对象，多个线程将同时访问并修改该计数器。在没有 std::mutex 保护的情况下，计数器的值可能会因数据竞争而出现错误。通过在访问和修改计数器的代码块中添加互斥锁，我们可以确保每个线程按顺序访问该资源，避免数据竞争。

#include <iostream>
#include <memory>
#include <thread>
#include <mutex>
#include <vector>

class Counter {
public:
    int value;

    Counter() : value(0) {}
    void increment() {
        ++value;
    }
    int getValue() const {
        return value;
    }
};

void thread_func(std::shared_ptr<Counter> counter, std::mutex& mtx) {
    for (int i = 0; i < 100; ++i) {
        std::lock_guard<std::mutex> lock(mtx);  // 加锁保护对 counter 的访问
        counter->increment();
    }
}

int main() {
    auto counter = std::make_shared<Counter>();
    std::mutex mtx;

    std::vector<std::thread> threads;

    // 启动10个线程，每个线程对 counter 执行 100 次 increment 操作
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back(thread_func, counter, std::ref(mtx));
    }

    // 等待所有线程完成
    for (auto& t : threads) {
        t.join();
    }

    std::cout << "Final counter value: " << counter->getValue() << std::endl; // 期望输出 1000

    return 0;
}
在这个例子中，Counter 类的对象由 std::shared_ptr 管理，并在多个线程中共享，在 thread_func 函数中，每次调用 counter->increment() 前，都用 std::lock_guard<std::mutex> 锁定 mtx，保证每次访问 increment() 是原子操作，std::lock_guard 是 RAII 风格的锁管理器，它会在代码块结束时自动释放锁。启动 10 个线程，每个线程对共享计数器执行 100 次增量操作。通过 std::mutex，我们保证了计数器的修改是线程安全的。

程序输出：Final counter value: 1000；在没有互斥锁的情况下，counter->increment() 在多个线程中可能会发生竞争，导致最终计数值低于预期的 1000。使用 std::mutex 来保护对共享资源的访问，保证了线程安全，确保最终计数器值为 1000。


来源：https://mp.weixin.qq.com/s/QIYuJQU-6cJC8kjk0pn5ew

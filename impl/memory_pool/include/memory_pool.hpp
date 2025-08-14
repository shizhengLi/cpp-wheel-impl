#pragma once

#include <cstddef>
#include <vector>
#include <list>
#include <mutex>
#include <memory>
#include <stdexcept>
#include <cassert>

namespace impl {

/**
 * @brief 内存池异常类
 */
class memory_pool_exception : public std::runtime_error {
public:
    explicit memory_pool_exception(const std::string& msg) : std::runtime_error(msg) {}
};

/**
 * @brief 固定大小内存池实现
 * 
 * 特点：
 * - 固定大小的内存块分配
 * - 线程安全的内存管理
 * - 高效的内存复用
 * - 支持内存块统计和调试
 */
class memory_pool {
public:
    /**
     * @brief 构造函数
     * @param block_size 内存块大小（字节）
     * @param initial_blocks 初始内存块数量
     * @param max_blocks 最大内存块数量（0表示无限制）
     */
    explicit memory_pool(size_t block_size, 
                       size_t initial_blocks = 32, 
                       size_t max_blocks = 0)
        : block_size_(block_size)
        , max_blocks_(max_blocks)
        , allocated_blocks_(0)
        , free_blocks_(0) {
        
        if (block_size == 0) {
            throw memory_pool_exception("Block size must be greater than 0");
        }
        
        // 确保块大小至少能存储一个指针（用于链表管理）
        if (block_size_ < sizeof(void*)) {
            block_size_ = sizeof(void*);
        }
        
        expand_pool(initial_blocks);
    }
    
    /**
     * @brief 析构函数，释放所有内存
     */
    ~memory_pool() {
        std::lock_guard<std::mutex> lock(mutex_);
        for (void* chunk : chunks_) {
            ::operator delete(chunk);
        }
    }
    
    /**
     * @brief 分配一个内存块
     * @return 分配的内存块指针
     * @throws memory_pool_exception 内存不足时抛出异常
     */
    void* allocate() {
        std::lock_guard<std::mutex> lock(mutex_);
        
        // 检查是否达到最大块数限制
        if (max_blocks_ > 0 && allocated_blocks_ >= max_blocks_) {
            throw memory_pool_exception("Memory pool exhausted: maximum blocks reached");
        }
        
        // 如果空闲链表为空，尝试扩展池
        if (free_list_.empty()) {
            expand_pool(chunks_.empty() ? 32 : chunks_.size());
        }
        
        // 从空闲链表中取出一个块
        void* block = free_list_.front();
        free_list_.pop_front();
        
        allocated_blocks_++;
        free_blocks_--;
        
        return block;
    }
    
    /**
     * @brief 释放内存块
     * @param block 要释放的内存块指针
     */
    void deallocate(void* block) {
        if (!block) {
            return;
        }
        
        std::lock_guard<std::mutex> lock(mutex_);
        
        // 验证块是否属于这个池（简化实现）
        // 在实际应用中，可能需要更复杂的验证机制
        
        // 将块放回空闲链表
        free_list_.push_front(block);
        
        allocated_blocks_--;
        free_blocks_++;
    }
    
    /**
     * @brief 检查内存池是否为空
     * @return 如果没有分配的块则返回true
     */
    bool empty() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return allocated_blocks_ == 0;
    }
    
    /**
     * @brief 获取已分配的块数量
     * @return 已分配的块数量
     */
    size_t allocated_count() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return allocated_blocks_;
    }
    
    /**
     * @brief 获取空闲的块数量
     * @return 空闲的块数量
     */
    size_t free_count() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return free_blocks_;
    }
    
    /**
     * @brief 获取总块数量
     * @return 总块数量
     */
    size_t total_count() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return allocated_blocks_ + free_blocks_;
    }
    
    /**
     * @brief 获取块大小
     * @return 每个块的大小（字节）
     */
    size_t block_size() const {
        return block_size_;
    }
    
    /**
     * @brief 获取最大块数量
     * @return 最大块数量（0表示无限制）
     */
    size_t max_blocks() const {
        return max_blocks_;
    }
    
    /**
     * @brief 收缩内存池，释放未使用的内存
     * @param min_blocks_to_keep 保留的最小块数量
     */
    void shrink(size_t min_blocks_to_keep = 0) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (total_count() <= min_blocks_to_keep) {
            return;
        }
        
        size_t blocks_to_remove = total_count() - min_blocks_to_keep;
        size_t removed_count = 0;
        
        // 从chunks_中释放内存块
        auto it = chunks_.begin();
        while (it != chunks_.end() && removed_count < blocks_to_remove) {
            void* chunk = *it;
            
            // 检查这个chunk中的块是否都在空闲链表中
            bool all_free = true;
            char* chunk_start = static_cast<char*>(chunk);
            char* chunk_end = chunk_start + chunk_size_;
            
            for (char* block = chunk_start; block < chunk_end; block += block_size_) {
                bool found = false;
                for (void* free_block : free_list_) {
                    if (free_block == block) {
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    all_free = false;
                    break;
                }
            }
            
            if (all_free) {
                // 从空闲链表中移除这个chunk的所有块
                for (char* block = chunk_start; block < chunk_end; block += block_size_) {
                    free_list_.remove(block);
                    free_blocks_--;
                }
                
                ::operator delete(chunk);
                it = chunks_.erase(it);
                removed_count++;
            } else {
                ++it;
            }
        }
    }
    
    /**
     * @brief 获取内存使用统计信息
     * @return 统计信息字符串
     */
    std::string get_stats() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return "MemoryPool Stats:\n"
               "  Block size: " + std::to_string(block_size_) + " bytes\n"
               "  Allocated blocks: " + std::to_string(allocated_blocks_) + "\n"
               "  Free blocks: " + std::to_string(free_blocks_) + "\n"
               "  Total blocks: " + std::to_string(total_count()) + "\n"
               "  Memory chunks: " + std::to_string(chunks_.size()) + "\n"
               "  Max blocks: " + (max_blocks_ > 0 ? std::to_string(max_blocks_) : "unlimited");
    }
    
    // 禁用拷贝构造和拷贝赋值
    memory_pool(const memory_pool&) = delete;
    memory_pool& operator=(const memory_pool&) = delete;
    
    // 允许移动构造和移动赋值
    memory_pool(memory_pool&& other) noexcept 
        : block_size_(other.block_size_)
        , chunk_size_(other.chunk_size_)
        , max_blocks_(other.max_blocks_)
        , allocated_blocks_(other.allocated_blocks_)
        , free_blocks_(other.free_blocks_)
        , chunks_(std::move(other.chunks_))
        , free_list_(std::move(other.free_list_)) {
        
        other.allocated_blocks_ = 0;
        other.free_blocks_ = 0;
        other.block_size_ = 0;
        other.chunk_size_ = 0;
        other.max_blocks_ = 0;
    }
    
    memory_pool& operator=(memory_pool&& other) noexcept {
        if (this != &other) {
            // 释放当前资源
            {
                std::lock_guard<std::mutex> lock(mutex_);
                for (void* chunk : chunks_) {
                    ::operator delete(chunk);
                }
            }
            
            // 移动其他资源
            std::lock_guard<std::mutex> lock(mutex_);
            block_size_ = other.block_size_;
            chunk_size_ = other.chunk_size_;
            max_blocks_ = other.max_blocks_;
            allocated_blocks_ = other.allocated_blocks_;
            free_blocks_ = other.free_blocks_;
            chunks_ = std::move(other.chunks_);
            free_list_ = std::move(other.free_list_);
            
            // 重置other
            other.allocated_blocks_ = 0;
            other.free_blocks_ = 0;
            other.block_size_ = 0;
            other.chunk_size_ = 0;
            other.max_blocks_ = 0;
        }
        return *this;
    }

private:
    /**
     * @brief 扩展内存池，分配新的内存块
     * @param blocks_to_add 要添加的块数量
     */
    void expand_pool(size_t blocks_to_add) {
        // 计算chunk大小（对齐到页面大小通常更高效）
        chunk_size_ = blocks_to_add * block_size_;
        
        // 分配新的内存chunk
        void* chunk = ::operator new(chunk_size_);
        chunks_.push_back(chunk);
        
        // 将chunk分割成块并添加到空闲链表
        char* block_start = static_cast<char*>(chunk);
        for (size_t i = 0; i < blocks_to_add; ++i) {
            void* block = block_start + i * block_size_;
            free_list_.push_back(block);
            free_blocks_++;
        }
    }
    
    size_t block_size_;          // 每个内存块的大小
    size_t chunk_size_;           // 每个内存chunk的大小
    size_t max_blocks_;           // 最大块数量限制
    size_t allocated_blocks_;     // 已分配的块数量
    size_t free_blocks_;          // 空闲的块数量
    
    std::vector<void*> chunks_;   // 内存chunks列表
    std::list<void*> free_list_;  // 空闲块链表
    mutable std::mutex mutex_;    // 互斥锁，保证线程安全
};

/**
 * @brief 内存池分配器适配器
 * 
 * 将memory_pool适配为STL风格的分配器接口
 */
template <typename T>
class memory_pool_allocator {
public:
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    
    template <typename U>
    struct rebind {
        using other = memory_pool_allocator<U>;
    };
    
    /**
     * @brief 构造函数
     * @param pool 内存池指针
     */
    explicit memory_pool_allocator(memory_pool* pool = nullptr) : pool_(pool) {}
    
    /**
     * @brief 拷贝构造函数
     */
    template <typename U>
    memory_pool_allocator(const memory_pool_allocator<U>& other) : pool_(other.pool_) {}
    
    /**
     * @brief 分配内存
     * @param n 要分配的元素数量
     * @return 分配的内存指针
     */
    pointer allocate(size_type n) {
        if (n != 1) {
            // 对于多个元素，使用标准分配器
            return static_cast<pointer>(::operator new(n * sizeof(T)));
        }
        
        if (!pool_) {
            throw memory_pool_exception("Memory pool not initialized");
        }
        
        return static_cast<pointer>(pool_->allocate());
    }
    
    /**
     * @brief 释放内存
     * @param p 要释放的内存指针
     * @param n 释放的元素数量
     */
    void deallocate(pointer p, size_type n) {
        if (n != 1) {
            // 对于多个元素，使用标准释放器
            ::operator delete(p);
            return;
        }
        
        if (pool_) {
            pool_->deallocate(p);
        }
    }
    
    /**
     * @brief 构造对象
     */
    template <typename U, typename... Args>
    void construct(U* p, Args&&... args) {
        new (p) U(std::forward<Args>(args)...);
    }
    
    /**
     * @brief 销毁对象
     */
    template <typename U>
    void destroy(U* p) {
        p->~U();
    }
    
    /**
     * @brief 获取内存池指针
     */
    memory_pool* get_pool() const { return pool_; }
    
private:
    memory_pool* pool_;
    
    template <typename U>
    friend class memory_pool_allocator;
};

template <typename T, typename U>
bool operator==(const memory_pool_allocator<T>& a, const memory_pool_allocator<U>& b) {
    return a.get_pool() == b.get_pool();
}

template <typename T, typename U>
bool operator!=(const memory_pool_allocator<T>& a, const memory_pool_allocator<U>& b) {
    return !(a == b);
}

} // namespace impl
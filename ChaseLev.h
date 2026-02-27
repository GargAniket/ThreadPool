#include <vector> 
#include <atomic> 

template<class T> 
class ChaseLev {
private: 
    alignas(64) std::vector<T> data_; 
    alignas(64) std::atomic<int> head_; 
    alignas(64) std::atomic<int> tail_; 
    int capacity_; 

public:
    ChaseLev(int size) : capacity_(size), head_(0), tail_(0) {
        data_.resize(capacity_); 
    }

    /**
     * @brief Push to bottom/tail
     * 
     * @param item The item to push
     * @return The success of the operation 
     */
    bool try_pushTail(T item) {
        int curr_tail = tail_.load(std::memory_order_relaxed); 
        int next_tail = (curr_tail + 1) % capacity_; 
        if ( next_tail == head_.load(std::memory_order_acquire) ) {
            return false; 
        }
        data_[curr_tail] = item; 
        // memory_order_release so writing to data_ before tail_ update
        tail_.store(next_tail, std::memory_order_release); 
        return true; 
    }

    /**
     * @brief Pop from bottom/tail
     * 
     * @param item The item to pop
     * @return The success of the operation
     */
    bool try_popTail(T& item) {

    }

    /**
     * @brief Steal from top/head
     * 
     * @param item The item to pop
     * @return The success of the operation
     */
    bool try_popHead(T& item) {
        int curr_head = head_.load(std::memory_order_relaxed); 
        if ( curr_head == tail_.load(std::memory_order_acquire) ) {
            return false; 
        } 
        item = data_[curr_head]; 
        int new_head = (curr_head + 1) % capacity_; 
        // memory_order_release so reading from data_ before head_ update 
        head_.store(new_head, std::memory_order_release); 
        return true; 
    }
};
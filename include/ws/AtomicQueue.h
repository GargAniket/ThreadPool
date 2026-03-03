#include <vector> 
#include <atomic> 

template<class T> 
class AtomicQueue {
private: 
    alignas(64) std::vector<T> data_; 
    alignas(64) std::atomic<int> head_; 
    alignas(64) std::atomic<int> tail_; 
    int capacity_; 

public:
    AtomicQueue(int size) : capacity_(size), head_(0), tail_(0) {
        data_.resize(capacity_); 
    }

    // push to tail
    bool try_enqueue(T item) {
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

    // pop from head 
    bool try_dequeue(T& item) {
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
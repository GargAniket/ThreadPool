#include <vector> 
#include <atomic> 

template<class T> 
class ChaseLev {
private: 
    alignas(64) std::vector<T> data_;   // NOTE: vector may reallocate in the future, use unique_ptr<T[]>
    alignas(64) std::atomic<int> head_; 
    alignas(64) std::atomic<int> tail_; 
    int capacity_; 
    int mask_; 

public:
    ChaseLev(int size) : capacity_(size), head_(0), tail_(0) {
        assert( (capacity_ > 0) && ((capacity_ & (capacity_ - 1)) == 0) && "size must be non-zero power of two" ); 
        mask_ = capacity_ - 1; 
        data_.resize(capacity_); 
    }

    /**
     * @brief Push to bottom/tail
     * 
     * Used by "owner" thread
     * 
     * @param item The item to push
     * @return The success of the operation 
     */
    bool try_pushTail(T item) {
        int curr_tail = tail_.load(std::memory_order_relaxed); 
        int curr_head = head_.load(std::memory_order_acquire); 
        if ( curr_tail - curr_head >= capacity_ ) {
            return false;   // full
        }
        data_[curr_tail & mask_] = item; 
        tail_.store(curr_tail + 1, std::memory_order_release); 
        return true; 
    }

    /**
     * @brief Pop from bottom/tail
     * 
     * Used by "owner" thread
     * 
     * @param item The item to pop
     * @return The success of the operation
     */
    bool try_popTail(T& item) {
        int curr_tail = tail_.load(std::memory_order_relaxed) - 1; 
        tail_.store(curr_tail, std::memory_order_release); 

        // std::atomic_thread_fence(std::memory_order_seq_cst); 

        int curr_head = head_.load(std::memory_order_acquire); 
        int size = curr_tail - curr_head; 

        // empty
        if ( size < 0 ) {
            tail_.store(curr_head, std::memory_order_relaxed); 
            return false; 
        }
        // space exists to pop
        if ( size > 0 ) { 
            item = data_[curr_tail & mask_];
            return true; 
        } 
        item = data_[curr_tail & mask_];
        // single element left (top/head == bottom/tail)
        // use CAS to resolve race between thief thread(s) popping from top/head and owner thread popping from bottom/tail
        if ( head_.compare_exchange_strong(curr_head, curr_head + 1, 
                                           std::memory_order_acq_rel, 
                                           std::memory_order_relaxed) ) {   // should this be std::memory_order_acquire or std::memory_order_relaxed ?
            tail_.store(curr_head + 1, std::memory_order_relaxed); 
            return true; 
        } else {
            tail_.store(curr_head + 1, std::memory_order_relaxed); 
            return false; 
        }
    }

    /**
     * @brief Steal from top/head
     * 
     * Used by "thief" threads
     * 
     * @param item The item to pop
     * @return The success of the operation
     */
    bool try_popHead(T& item) {
        int curr_head = head_.load(std::memory_order_acquire); 
        int curr_tail = tail_.load(std::memory_order_acquire); 
        if ( curr_head >= curr_tail ) {
            return false;   // empty
        }
        item = data_[curr_head & mask_];
        // use CAS to resolve race between multiple thief threads colliding/popping from same top/head
        if ( head_.compare_exchange_strong(curr_head, curr_head+1, 
                                           std::memory_order_acq_rel, 
                                           std::memory_order_relaxed) ) {   // should this be std::memory_order_acquire or std::memory_order_relaxed ? 
            return true; 
        } else {
            return false;   // retry
        }
    }
};

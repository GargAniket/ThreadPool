#include <vector> 
#include <mutex> 

template <class T> 
class LockingQueue {
private: 
    std::vector<T> data_; 
    std::mutex mutex_; 
    int head_; 
    int tail_;
    int capacity_; 
    int count_;  

    std::condition_variable not_full_; 
    std::condition_variable not_empty_; 

public: 
    LockingQueue(int size) : capacity_(size), head_(0), tail_(0), count_(0) {
        data_.resize(capacity_); 
    }

    void enqueue(const T& item) {
        std::unique_lock<std::mutex> lock(mutex_); 
        
        // while (count_ != capacity_) { not_full_.wait(lock); }
        not_full_.wait(lock, [this] { return count_ != capacity_; }); 

        // same as non-concurrent enqueue
        data_[tail_] = item; 
        tail_ = (tail_ + 1) % capacity_;    // NOTE: use (tail_ + 1) & (capacity_ - 1) if capacity_ is power of 2
        count_++; 

        not_empty_.notify_one(); 
    }

    void dequeue(T& item) {
        std::unique_lock<std::mutex> lock(mutex_); 
        
        // while ( count_ != 0 ) { not_empty_.wait(lock); }
        not_empty_.wait(lock, [this] { return count_ != 0; }); 

        // same as non-concurrent dequeue
        item = data_[head_]; 
        head_ = (head_ + 1) % capacity_;    // NOTE: use (tail_ + 1) & (capacity_ - 1) if capacity_ is power of 2
        count_--; 

        not_full_.notify_one(); 
    }

    int getCount() {
        return count_; 
    }
};

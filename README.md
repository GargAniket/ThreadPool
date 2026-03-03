# **ThreadPool**
This is intended to be a demonstration of a thread pool based on Chase-Lev. Resources, documentation, configuration, and benchmarks will be added over time. This is currently a very rough draft, and the project is (over)due for some big shuffling around going forwards :D

# **TODO List:**
- [X] repo structure
- [X] implement a bounded blocking queue
- [ ] implement Chase-Lev dequeue
- [ ] verify MPSC/MPMC/SPSC/SPMC correctness
- [ ] create class ThreadPool
- [ ] implement mutex-based work stealing
- [ ] imlement lock-free work stealing
- [ ] benchmarks, flamegraphs, latency histograms, memory layout discussion, lock contention analysis, etc

# **Stretch Goals:**
- [ ] NUMA-aware scheduling
- [ ] batched task enqueue/dequeue
- [ ] backoff strategies
- [ ] task priority levels (how to stop priority inversion, deadlocks, livelocks, etc)
- [ ] memory reclamation (hazard pointers of epoch-based garbage collection)
- [ ] false sharing mitigation

# **Notes:** 
- [ ] how to handle memory reclamation? 
- [ ] how to handle overflow?
- [ ] what if consumer thread(s) are too slow/crash? 
- [ ] theives pop from tail, owner pops/pushes to bottom 

# **Design Decsions:**
- Chase-Lev Deque
    - use fixed-size backing array --> must implement backoff strategy, dont want to deal with resizing since it can lead to issues if consumer thread(s) crash or too many tasks pushed, also deterministic since no heap allocation
    - arithmetic and-based indexing --> faster than modulo (compiler optimizations could also do this for you)

# **Microbenchmarks:**
- [ ] single owner + 3 thief threads + fixed capacity of 8 + push integers. Track atomic counter of items produced, atomic counter of items consumed, and bitmap to ensure no duplicates 
- [ ] single worker --> measure ns per push/pop vs mutex queue, mpmc queue, chase lev deque
- [ ] 1 producer worker + N thieves --> preload 1M tasks + measure steals/sec as N increases 
- [ ] collect per-task execution latency (p50, p90, p99, p99.9, max)
- [ ] collect LLC misses
- [ ] collect branch mispredicts
- [ ] collect CAS failure rate
- [ ] collect context switches 
- [ ] plot throughput vs threads (linear region, saturation point, contention cliff)

# **Debugging:**
-fsanitize=address
-fsanitize=thread
-O1 (not O3)

# **Standard Build:** 
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .

# **Debug Build with Sanitizers:**
cmake .. \
  -DCMAKE_BUILD_TYPE=Debug \
  -DWS_ENABLE_SANITIZERS=ON

cmake --build .

# **Run Tests:**
./examples/example_map_reduce
./examples/example_recursive_fib
./examples/example_parallel_quicksort
./tests/ws_tests
./benchmarks/ws_benchmarks
./apps/histogram_app

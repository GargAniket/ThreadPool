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
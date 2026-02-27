# **TODO LIST:**
- [ ] repo structure
- [ ] implement a bounded blocking queue
- [ ] explore modulo vs bit level arithemtic for head/tail update
- [ ] implement Chase-Lev dequeue
- [ ] verify MPSC/MPMC/SPSC/SPMC correctness
- [ ] create class ThreadPool
- [ ] implement mutex-based work stealing
- [ ] imlement lock-free work stealing
- [ ] benchmarks, flamegraphs, latency histograms, memory layout discussion, lock contention analysis, etc

# **STRETCH GOALS:**
- [ ] NUMA-aware scheduling
- [ ] batched task enqueue/dequeue
- [ ] backoff strategies
- [ ] task priority levels (how to stop priority inversion, deadlocks, livelocks, etc)
- [ ] memory reclamation (hazard pointers of epoch-based garbage collection)
- [ ] false sharing mitigation

# **NOTES:** 
- [ ] how to handle memory reclamation? 
- [ ] how to handle overflow?
- [ ] what if consumer thread(s) are too slow/crash? 
- [ ] theives pop from tail, owner pops/pushes to bottom 
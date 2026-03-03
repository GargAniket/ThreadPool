add_executable(ws_benchmarks
    benchmark_main.cpp
    parallel_for_bench.cpp
    fib_bench.cpp
    scaling_bench.cpp
)

target_link_libraries(ws_benchmarks PRIVATE ws)
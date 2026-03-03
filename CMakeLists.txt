cmake_minimum_required(VERSION 3.14)

project(ThreadPool 
    VERSION 1.0
    LANGUAGES CXX
)

# ----------------------------
# Global Options
# ----------------------------

option(WS_BUILD_TESTS "Build Tests" ON)
option(WS_BUILD_BENCHMARKS "Build Benchmarks" ON)
option(WS_ENABLE_SANITIZERS "Enable Sanitizers" OFF)
option(WS_ENABLE_WARNINGS "Enable Extra Compiler Warnings" ON)

# ----------------------------
# C++ Standard
# ----------------------------

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ----------------------------
# Library Target
# ----------------------------

add_library(ws
    src/AtomicQueue.cpp
    src/ChaseLev.cpp
    src/LockingQueue.cpp
    src/thread_pool.cpp
    src/worker.cpp
)

target_include_directories(ws
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_compile_features(ws PUBLIC cxx_std_20)

# ----------------------------
# Compiler Warnings
# ----------------------------

if (WS_ENABLE_WARNINGS)
    if (MSVC)
        target_compile_options(ws PRIVATE /W4 /permissive-)
    else()
        target_compile_options(ws PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -Wconversion
            -Wshadow
            -Wnon-virtual-dtor
            -Wold-style-cast
            -Wcast-align
            -Wunused
            -Woverloaded-virtual
            -Wnull-dereference
            -Wdouble-promotion
            -Wformat=2
        )
    endif()
endif()

# ----------------------------
# Sanitizers
# ----------------------------

if (WS_ENABLE_SANITIZERS AND NOT MSVC)
    message(STATUS "Sanitizers enabled")

    target_compile_options(ws PRIVATE
        -fsanitize=address
        -fsanitize=undefined
        -fsanitize=thread
        -fno-omit-frame-pointer
    )

    target_link_options(ws PRIVATE
        -fsanitize=address
        -fsanitize=undefined
        -fsanitize=thread
    )
endif()

# ----------------------------
# Threads
# ----------------------------

find_package(Threads REQUIRED)
target_link_libraries(ws PUBLIC Threads::Threads)

# ----------------------------
# Subdirectories
# ----------------------------

if (WS_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

if (WS_BUILD_BENCHMARKS)
    add_subdirectory(benchmarks)
endif()

add_subdirectory(examples)
add_subdirectory(apps)
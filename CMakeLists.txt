cmake_minimum_required(VERSION 3.14)
project(ThreadPool LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)

find_package(OpenCV REQUIRED)

add_executable(ThreadPool 
                main.cpp
                AtomicQueue.cpp
                LockingQueue.cpp
                )
target_include_directories(ThreadPool PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(ThreadPool PRIVATE ${OpenCV_LIBS})
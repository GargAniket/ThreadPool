find_package(OpenCV REQUIRED)

add_executable(ws_tests
    thread_pool_tests.cpp
    deque_tests.cpp
    stress_tests.cpp
)

target_link_libraries(ws_tests PRIVATE ws ${OpenCV_LIBS})

add_test(NAME ws_tests COMMAND ws_tests)

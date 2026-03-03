find_package(OpenCV REQUIRED)

add_executable(histogram_app histogram.cpp)

target_link_libraries(histogram_app
    PRIVATE
        ws 
        ${OpenCV_LIBS}
)

target_include_directories(histogram_app
    PRIVATE
        ${OpenCV_INCLUDE_DIRS}
)
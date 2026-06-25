cmake_minimum_required(VERSION 3.10)
project(kuwahara_filter)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(OpenCV_DIR "C:/Users/agnaw/OneDrive/Documents/opencv/build")
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(kuwahara_filter kuwahara_filter.cpp)
target_include_directories(kuwahara_filter PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(kuwahara_filter PRIVATE ${OpenCV_LIBS})
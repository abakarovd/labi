cmake_minimum_required(VERSION 3.20)
project(laba1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(task1 task1.cpp)
add_executable(task2 task2.cpp)

target_include_directories(task1 PRIVATE /opt/homebrew/include)
target_link_directories(task1 PRIVATE /opt/homebrew/lib)
target_link_libraries(task1 PRIVATE boost_thread boost_system)

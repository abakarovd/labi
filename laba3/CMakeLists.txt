cmake_minimum_required(VERSION 3.20)
project(lab3_variant1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost REQUIRED)

add_executable(task1_sync_server
    task1_sync/sync_server.cpp
)

add_executable(task1_sync_client
    task1_sync/sync_client.cpp
)

add_executable(async_server
    async_server/main_server.cpp
    async_server/server.cpp
)

add_executable(async_client
    async_server/main_client.cpp
    async_server/client.cpp
)

target_include_directories(task1_sync_server PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories(task1_sync_client PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories(async_server PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories(async_client PRIVATE ${Boost_INCLUDE_DIRS})
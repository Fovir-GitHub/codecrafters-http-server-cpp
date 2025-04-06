cmake_minimum_required(VERSION 3.13)

project(http-server-starter-cpp)

file(GLOB_RECURSE SOURCE_FILES src/*.cpp src/*.hpp)

set(CMAKE_CXX_STANDARD 23) # Enable the C++23 standard
set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package(Threads REQUIRED)
find_package(ZLIB REQUIRED)

set(CMAKE_BUILD_TYPE Debug)

add_subdirectory(src)
add_executable(server src/main.cpp)

target_link_libraries(server PRIVATE server_module http_module Threads::Threads ZLIB::ZLIB)

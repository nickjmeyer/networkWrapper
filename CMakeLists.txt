cmake_minimum_required(VERSION 3.5)
project(networkWrapper)

find_package(Boost COMPONENTS system filesystem thread REQUIRED)
find_package(Threads REQUIRED)

set(CMAKE_C_FLAGS "-std=c++11")

add_subdirectory(src/main)

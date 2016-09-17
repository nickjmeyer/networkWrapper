cmake_minimum_required(VERSION 3.5)
project(networkWrapper)

find_package(Boost COMPONENTS system filesystem REQUIRED)

set(CMAKE_C_FLAGS "-std=c++11")

include_directories(src/main ${Boost_INCLUDE_DIR})
add_library(networkWrapper src/main/networkWrapper.cpp)
target_link_libraries(networkWrapper ${Boost_FILESYSTEM_LIBRARY})

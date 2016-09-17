add_library(networkWrapper networkWrapper.cpp)
target_link_libraries(networkWrapper ${Boost_FILESYSTEM_LIBRARY})

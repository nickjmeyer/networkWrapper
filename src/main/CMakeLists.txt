add_library(networkWrapper networkWrapper.cpp)
target_link_libraries(networkWrapper ${Boost_FILESYSTEM_LIBRARY}
	${Boost_SYSTEM_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})

add_executable(echoServer echoServer.cpp)
target_link_libraries(echoServer networkWrapper ${Boost_THREAD_LIBRARY} )

add_executable(echoClient echoClient.cpp)
target_link_libraries(echoClient networkWrapper ${Boost_THREAD_LIBRARY} )

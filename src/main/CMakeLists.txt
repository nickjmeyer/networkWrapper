add_library(networkWrapper networkWrapper.cpp)
target_link_libraries(networkWrapper ${Boost_FILESYSTEM_LIBRARY}
	${Boost_SYSTEM_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS chat.proto)

# echo binaries
add_executable(echoServer echoServer.cpp)
target_link_libraries(echoServer networkWrapper ${Boost_THREAD_LIBRARY})

add_executable(echoClient echoClient.cpp)
target_link_libraries(echoClient networkWrapper ${Boost_THREAD_LIBRARY})

# chat binaries
add_executable(chatServer chatServer.cpp ${PROTO_SRCS} ${PROTO_HDRS})
target_link_libraries(chatServer networkWrapper ${Boost_THREAD_LIBRARY}
	${PROTOBUF_LIBRARY})

add_executable(chatClient chatClient.cpp ${PROTO_SRCS} ${PROTO_HDRS})
target_link_libraries(chatClient networkWrapper ${Boost_THREAD_LIBRARY}
	${PROTOBUF_LIBRARY})

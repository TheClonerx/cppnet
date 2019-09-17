cmake_minimum_required(VERSION 3.8)
project(cppnet)

find_package(Threads)

include_directories(${CMAKE_SOURCE_DIR}/include)

add_library(cppnet src/epoll.cpp src/getaddrinfo.cpp src/poll.cpp src/select.cpp src/socket_common_impl.cpp)
target_compile_features(cppnet PUBLIC cxx_std_17)
if(WIN32)
	target_sources(cppnet PRIVATE src/socket_win32_impl.cpp)
	target_link_libraries(cppnet wsock32 ws2_32)
else()
	target_sources(cppnet PRIVATE src/socket_unix_impl.cpp)
endif()

add_executable(example_getaddrinfo src/examples/example_getaddrinfo.cpp)
target_compile_features(example_getaddrinfo PUBLIC cxx_std_17)
target_link_libraries(example_getaddrinfo cppnet ${CMAKE_THREAD_LIBS_INIT})

add_executable(example_poll src/examples/example_poll.cpp)
target_compile_features(example_poll PUBLIC cxx_std_17)
target_link_libraries(example_poll cppnet)

add_executable(example_select src/examples/example_select.cpp)
target_compile_features(example_select PUBLIC cxx_std_17)
target_link_libraries(example_select cppnet)
cmake_minimum_required(VERSION 3.14)
project(cppnet)

include_directories(${CMAKE_SOURCE_DIR}/include)
add_library(cppnet src/epoll.cpp src/getaddrinfo.cpp src/poll.cpp src/select.cpp src/socket.cpp src/utils.cpp)
target_compile_features(cppnet PUBLIC cxx_std_17)

add_executable(example_getaddrinfo src/examples/example_getaddrinfo.cpp)
target_compile_features(example_getaddrinfo PUBLIC cxx_std_17)
target_link_libraries(example_getaddrinfo cppnet pthread)

add_executable(example_poll src/examples/example_poll.cpp)
target_compile_features(example_poll PUBLIC cxx_std_17)
target_link_libraries(example_poll cppnet)

add_executable(example_select src/examples/example_select.cpp)
target_compile_features(example_select PUBLIC cxx_std_17)
target_link_libraries(example_select cppnet)
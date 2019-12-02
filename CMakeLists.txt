cmake_minimum_required(VERSION 3.8)
project(cppnet)

find_package(Threads)

include_directories(include)

add_library(cppnet)
target_include_directories(cppnet PUBLIC include)
target_compile_features(cppnet PUBLIC cxx_std_17)
target_sources(cppnet PRIVATE
	src/getaddrinfo.cpp
	src/poll.cpp
	src/select.cpp
	src/socket_common_impl.cpp
	src/address.cpp
)

if(${CMAKE_HOST_SYSTEM_NAME} MATCHES "Linux")
	# Linux specific
	target_sources(cppnet PRIVATE src/epoll.cpp)
endif()

if(${CMAKE_HOST_SYSTEM_NAME} MATCHES "Windows")
	# Windows specific
	target_sources(cppnet PRIVATE src/socket_win32_impl.cpp)
	target_link_libraries(cppnet wsock32 ws2_32)
endif()

if(UNIX)
	# Unix specific
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

add_executable(example_http src/examples/example_http.cpp)
target_compile_features(example_http PUBLIC cxx_std_17)
target_link_libraries(example_http cppnet)

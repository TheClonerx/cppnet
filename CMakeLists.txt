cmake_minimum_required(VERSION 3.8)
project(cppnet)

option(CPPNET_BUILD_EXAMPLES "Build the cppnet examples" OFF)

# include_directories(include)

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


if (CPPNET_BUILD_EXAMPLES)
	add_subdirectory(examples)
endif()

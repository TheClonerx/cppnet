find_package(Threads)

add_executable(example_getaddrinfo example_getaddrinfo.cpp)
target_compile_features(example_getaddrinfo PUBLIC cxx_std_17)
target_link_libraries(example_getaddrinfo cppnet ${CMAKE_THREAD_LIBS_INIT} ntdll)

add_executable(example_poll example_poll.cpp)
target_compile_features(example_poll PUBLIC cxx_std_17)
target_link_libraries(example_poll cppnet)

add_executable(example_select example_select.cpp)
target_compile_features(example_select PUBLIC cxx_std_17)
target_link_libraries(example_select cppnet)

add_executable(example_http example_http.cpp)
target_compile_features(example_http PUBLIC cxx_std_17)
target_link_libraries(example_http cppnet)

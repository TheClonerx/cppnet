cmake_minimum_required(VERSION 3.8)

project(cppnet VERSION 1.1.3 LANGUAGES CXX)

option(CPPNET_BUILD_EXAMPLES "Build the cppnet examples" OFF)

add_library(cppnet)
target_compile_features(cppnet PUBLIC cxx_std_17)
target_sources(cppnet PRIVATE
    src/getaddrinfo.cpp
    src/poll.cpp
    src/select.cpp
    src/socket_common_impl.cpp
    src/address.cpp
)

if(${CMAKE_HOST_SYSTEM_NAME} MATCHES "Linux") # Linux specific
    target_sources(cppnet PRIVATE src/epoll.cpp)
endif()

if(WIN32) # Windows specific
    target_sources(cppnet PRIVATE src/socket_win32_impl.cpp)
    target_link_libraries(cppnet wsock32 ws2_32)
endif()

if(UNIX) # Unix specific
    target_sources(cppnet PRIVATE src/socket_unix_impl.cpp)
endif()

target_compile_definitions(cppnet PRIVATE CPPNET_IMPL)

if (CPPNET_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

target_include_directories(
    cppnet PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}-${PROJECT_VERSION}/include/>
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/${PROJECT_NAME}-config-version.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMinorVersion
)

install(
    TARGETS cppnet
    EXPORT ${PROJECT_NAME}-config
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/${PROJECT_NAME}-${PROJECT_VERSION}/${PROJECT_NAME}
)

install(
    EXPORT ${PROJECT_NAME}-config
    NAMESPACE ${PROJECT_NAME}::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}-${PROJECT_VERSION}
)

install(
    DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}-${PROJECT_VERSION}/${PROJECT_NAME}
)

install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/${PROJECT_NAME}-config-version.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}-${PROJECT_VERSION}
)

install(
    FILES "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/${PROJECT_NAME}-${PROJECT_VERSION}
)

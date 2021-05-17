cmake_minimum_required(VERSION 3.8)

project(cppnet VERSION 1.2.1 LANGUAGES CXX)

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
set_target_properties(cppnet PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(cppnet PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR})

if(${CMAKE_HOST_SYSTEM_NAME} MATCHES "Linux") # Linux specific
    target_sources(cppnet PRIVATE src/epoll.cpp)
endif()

if(WIN32 OR MINGW) # Windows specific
    target_sources(cppnet PRIVATE src/socket_win32_impl.cpp)
    target_link_libraries(cppnet PUBLIC wsock32 ws2_32)
endif()

if(UNIX AND NOT MINGW) # Unix specific
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
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}-${PROJECT_VERSION}/cppnet/>
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/${PROJECT_NAME}-config-version.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMinorVersion
)

install(
    TARGETS cppnet
    EXPORT ${PROJECT_NAME}-config
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/${PROJECT_NAME}-${PROJECT_VERSION}/"
)

install(
    EXPORT ${PROJECT_NAME}-config
    NAMESPACE ${PROJECT_NAME}::
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}-${PROJECT_VERSION}"
)

install(
    FILES 
        "${CMAKE_CURRENT_LIST_DIR}/include/cppnet/address.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/include/cppnet/getaddrinfo.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/include/cppnet/poll.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/include/cppnet/select.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/include/cppnet/socket.hpp"
    DESTINATION
        "${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}-${PROJECT_VERSION}/cppnet"
)

if(WIN32)
    install(
        FILES 
            "${CMAKE_CURRENT_LIST_DIR}/include/cppnet/wsa_init.hpp"
        DESTINATION
            "${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}-${PROJECT_VERSION}/cppnet"
    )
endif()

if(${CMAKE_HOST_SYSTEM_NAME} MATCHES "Linux")
    install(
        FILES 
            "${CMAKE_CURRENT_LIST_DIR}/include/cppnet/epoll.hpp"
        DESTINATION
            "${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}-${PROJECT_VERSION}/cppnet"
    )
endif()

install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/${PROJECT_NAME}-config-version.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}-${PROJECT_VERSION}"
)

install(
    FILES "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/${PROJECT_NAME}-${PROJECT_VERSION}"
)

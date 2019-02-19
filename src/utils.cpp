#include "net/utils.hpp"
#include <arpa/inet.h>
#include <cassert>
#include <cstring>
#include <sys/un.h>

std::string net::addr_to_str(int family, const sockaddr_storage* addr, std::error_code& e) noexcept
{
    assert(addr && "addr is null");
    char buff[1024] { 0 };
    const char* ret = nullptr;
    if (family == AF_INET)
        ret = inet_ntop(AF_INET, &((const sockaddr_in*)addr)->sin_addr, buff, sizeof(buff));
    else if (family == AF_INET6)
        ret = inet_ntop(AF_INET6, &((const sockaddr_in6*)addr)->sin6_addr, buff, sizeof(buff));
    else if (family == AF_UNIX) {
        std::memcpy(buff, ((const sockaddr_un*)addr)->sun_path, std::strlen(((const sockaddr_un*)addr)->sun_path));
        ret = buff;
    }

    if (!ret) {
        e.assign(errno, std::system_category());
        return {};
    } else {
        e.assign(0, std::system_category());
        return buff;
    }
}

std::string net::addr_to_str(int family, const sockaddr_storage* addr)
{
    assert(addr && "addr is null");
    char buff[1024] { 0 };
    const char* ret = nullptr;
    if (family == AF_INET)
        ret = inet_ntop(AF_INET, &((const sockaddr_in*)addr)->sin_addr, buff, sizeof(buff));
    else if (family == AF_INET6)
        ret = inet_ntop(AF_INET6, &((const sockaddr_in6*)addr)->sin6_addr, buff, sizeof(buff));
    else if (family == AF_UNIX) {
        std::memcpy(buff, ((const sockaddr_un*)addr)->sun_path, std::strlen(((const sockaddr_un*)addr)->sun_path));
        ret = buff;
    }
    if (!ret)
        throw std::system_error(errno, std::system_category());
    return buff;
}
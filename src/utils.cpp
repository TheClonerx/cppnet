#include "net/utils.hpp"
#include <arpa/inet.h>
#include <cassert>
#include <cstring>
#include <sys/un.h>

std::string net::addr_to_str(const sockaddr_storage* addr, std::error_code& e) noexcept
{
    char buff[1024] { 0 };
    const char* ret = nullptr;

    if (addr->ss_family == AF_INET)
        ret = inet_ntop(AF_INET, &reinterpret_cast<const sockaddr_in*>(addr)->sin_addr, buff, sizeof(buff));
    else if (addr->ss_family == AF_INET6)
        ret = inet_ntop(AF_INET6, &reinterpret_cast<const sockaddr_in6*>(addr)->sin6_addr, buff, sizeof(buff));
    else if (addr->ss_family == AF_UNIX)
        ret = std::strcpy(buff, reinterpret_cast<const sockaddr_un*>(addr)->sun_path);

    if (!ret) {
        e.assign(errno, std::system_category());
        return {};
    } else {
        e.assign(0, std::system_category());
        return buff;
    }
}

std::string net::addr_to_str(const sockaddr_storage* addr)
{
    char buff[1024] { 0 };
    const char* ret = nullptr;

    if (addr->ss_family == AF_INET)
        ret = inet_ntop(AF_INET, &reinterpret_cast<const sockaddr_in*>(addr)->sin_addr, buff, sizeof(buff));
    else if (addr->ss_family == AF_INET6)
        ret = inet_ntop(AF_INET6, &reinterpret_cast<const sockaddr_in6*>(addr)->sin6_addr, buff, sizeof(buff));
    else if (addr->ss_family == AF_UNIX)
        ret = std::strcpy(buff, reinterpret_cast<const sockaddr_un*>(addr)->sun_path);

    if (!ret)
        throw std::system_error(errno, std::system_category());
    return buff;
}
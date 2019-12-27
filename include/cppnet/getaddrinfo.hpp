#pragma once

#include <memory>
#include <system_error>

#include <sys/types.h>

#ifdef _WIN32
#include <WS2tcpip.h>
#include <WinSock2.h>
#else
#include <netdb.h> // struct addrinfo, getaddrinfo, freeaddrinfo, EAI_SYSTEM
#include <sys/socket.h>
#endif

#include <cppnet/address.hpp>

namespace net {

const std::error_category& addrinfo_category() noexcept;

class address_info {
public:
    address_info() noexcept = default;

    int family() const noexcept;
    int type() const noexcept;
    int protocol() const noexcept;
    const net::address& address() const noexcept;
    std::string_view canon_name() const noexcept;

private:
    template <typename It>
    friend It getaddrinfo(It, It, const char*, const char*, int, int, int, int, std::error_code&) noexcept;
    template <typename It>
    friend It getaddrinfo(It, const char*, const char*, int, int, int, int, std::error_code&) noexcept;

    address_info(int, int, int, const sockaddr*, size_t, const char*) noexcept;

private:
    int m_type = -1;
    int m_protocol = -1;
    net::address m_address;
    std::string m_canonname;
};

template <typename It>
It getaddrinfo(It start, It stop, const char* node, const char* service, int family, int type, int protocol, int flags, std::error_code& e) noexcept
{
    static_assert(std::is_assignable_v<decltype(*start), address_info>, "Iterator value type must be assignable to net::address_info");
    ::addrinfo hints{};

    hints.ai_family = family;
    hints.ai_socktype = type;
    hints.ai_protocol = protocol;
    hints.ai_flags = flags;

    ::addrinfo* addrlist{};
    int r = ::getaddrinfo(node, service, &hints, &addrlist);
#ifdef _WIN32
    if (r != 0) {
        e.assign(WSAGetLastError(), std::system_category());
        return start;
    }
#else
    if (r == EAI_SYSTEM) {
        e.assign(errno, std::system_category());
        return start;
    } else if (r != 0) {
        e.assign(r, addrinfo_category());
        return start;
    }
#endif
    else
        e.assign(0, std::system_category());

    for (::addrinfo* i = addrlist; i != nullptr && start != stop; i = i->ai_next, ++start) {
        *start = address_info(
            i->ai_family,
            i->ai_socktype,
            i->ai_protocol,
            i->ai_addr,
            i->ai_addrlen,
            i->ai_canonname);
    }
    ::freeaddrinfo(addrlist);
    return start;
}

template <typename It>
It getaddrinfo(It it, const char* node, const char* service, int family, int type, int protocol, int flags, std::error_code& e) noexcept
{
    static_assert(std::is_assignable_v<decltype(*it), address_info>, "Iterator value type must be assignable to net::address_info");
    ::addrinfo hints{};

    hints.ai_flags = flags;
    hints.ai_family = family;
    hints.ai_socktype = type;
    hints.ai_protocol = protocol;

    ::addrinfo* addrlist;
    int r = ::getaddrinfo(node, service, &hints, &addrlist);
#ifdef _WIN32
    if (r != 0) {
        e.assign(WSAGetLastError(), std::system_category());
        return it;
    }
#else
    if (r == EAI_SYSTEM) {
        e.assign(errno, std::system_category());
        return it;
    } else if (r != 0) {
        e.assign(r, addrinfo_category());
        return it;
    }
#endif
    else
        e.assign(0, std::system_category());

    for (::addrinfo* i = addrlist; i != nullptr; i = i->ai_next, ++it) {
        *it = address_info(
            i->ai_family,
            i->ai_socktype,
            i->ai_protocol,
            i->ai_addr,
            i->ai_addrlen,
            i->ai_canonname);
    }
    ::freeaddrinfo(addrlist);
    return it;
}

template <typename It>
It getaddrinfo(It start, It stop, const char* host, const char* service, int family = 0, int type = 0, int protocol = 0, int flags = 0)
{
    std::error_code e;
    auto ret = getaddrinfo(start, stop, host, service, family, type, protocol, flags, e);
    if (e)
        throw std::system_error(e);
    return ret;
}

template <typename It>
It getaddrinfo(It it, const char* host, const char* service, int family = 0, int type = 0, int protocol = 0, int flags = 0)
{
    std::error_code e;
    auto ret = getaddrinfo(it, host, service, family, type, protocol, flags, e);
    if (e)
        throw std::system_error(e);
    return ret;
}

inline address_info& getaddrinfo(address_info& ainfo, const char* host, const char* service, int family = 0, int type = 0, int protocol = 0, int flags = 0)
{
    getaddrinfo(&ainfo, &ainfo + 1, host, service, family, type, protocol, flags);
    return ainfo;
}

inline address_info& getaddrinfo(address_info& ainfo, const char* host, const char* service, int family, int type, int protocol, int flags, std::error_code& e) noexcept
{
    getaddrinfo(&ainfo, &ainfo + 1, host, service, family, type, protocol, flags, e);
    return ainfo;
}

inline address_info getaddrinfo(const char* host, const char* service, int family = 0, int type = 0, int protocol = 0, int flags = 0)
{
    address_info ainfo;
    return getaddrinfo(ainfo, host, service, family, type, protocol, flags);
}

inline address_info getaddrinfo(const char* host, const char* service, int family, int type, int protocol, int flags, std::error_code& e) noexcept
{
    address_info ainfo;
    return getaddrinfo(ainfo, host, service, family, type, protocol, flags, e);
}
}

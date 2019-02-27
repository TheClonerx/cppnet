#pragma once

#include <memory>
#include <sys/socket.h>
#include <system_error>

#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

namespace net {

const ::std::error_category& addrinfo_category();

struct addrinfo {
    int family;
    int type;
    int protocol;
    socklen_t addrlen;
    sockaddr_storage addr;
    std::string canonname;
};

template <typename It>
It getaddrinfo(It start, It stop, const char* node, const char* service, int family, int type, int protocol, int flags, std::error_code& e) noexcept
{
    static_assert(std::is_same<decltype(*start = addrinfo(), std::true_type()), std::true_type>::value, "Iterator value type must be assignable to net::addrinfo");
    ::addrinfo hints;
    hints.ai_flags = flags;
    hints.ai_family = family;
    hints.ai_socktype = type;
    hints.ai_protocol = protocol;

    ::addrinfo* addrlist;
    int r = ::getaddrinfo(node, service, &hints, &addrlist);
    if (r == EAI_SYSTEM) {
        e.assign(errno, std::system_category());
        return start;
    } else if (r != 0) {
        e.assign(r, addrinfo_category());
        return start;
    } else

        e.assign(0, std::system_category());

    sockaddr_storage tmp;

    for (::addrinfo* i = addrlist; i != nullptr && start != stop; i = i->ai_next, ++start) {
        // man pages doe
        std::copy(reinterpret_cast<char*>(i->ai_addr), reinterpret_cast<char*>(i->ai_addr) + i->ai_addrlen, reinterpret_cast<char*>(&tmp));
        std::fill_n(reinterpret_cast<char*>(&tmp) + i->ai_addrlen, sizeof(sockaddr_storage) - i->ai_addrlen, 0);
        *start = addrinfo {
            i->ai_family,
            i->ai_socktype,
            i->ai_protocol,
            i->ai_addrlen,
            std::move(tmp),
            i->ai_canonname ? i->ai_canonname : std::string()
        };
    }
    ::freeaddrinfo(addrlist);
    return start;
}

template <typename It>
It getaddrinfo(It it, const char* node, const char* service, int family, int type, int protocol, int flags, std::error_code& e) noexcept
{
    static_assert(std::is_same<decltype(*it = addrinfo(), std::true_type()), std::true_type>::value, "Iterator value type must be assignable to net::addrinfo");
    ::addrinfo hints;
    hints.ai_flags = flags;
    hints.ai_family = family;
    hints.ai_socktype = type;
    hints.ai_protocol = protocol;

    ::addrinfo* addrlist;
    int r = ::getaddrinfo(node, service, &hints, &addrlist);
    if (r == EAI_SYSTEM) {
        e.assign(errno, std::system_category());
        return it;
    } else if (r != 0) {
        e.assign(r, addrinfo_category());
        return it;
    } else {
        e.assign(0, std::system_category());
    }

    sockaddr_storage tmp;

    for (::addrinfo* i = addrlist; i != nullptr; i = i->ai_next, ++it) {
        std::copy(reinterpret_cast<char*>(i->ai_addr), reinterpret_cast<char*>(i->ai_addr) + i->ai_addrlen, reinterpret_cast<char*>(&tmp));
        std::fill_n(reinterpret_cast<char*>(&tmp) + i->ai_addrlen, sizeof(sockaddr_storage) - i->ai_addrlen, 0);
        *it = addrinfo {
            i->ai_family,
            i->ai_socktype,
            i->ai_protocol,
            i->ai_addrlen,
            std::move(tmp),
            i->ai_canonname ? i->ai_canonname : std::string()
        };
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

addrinfo& getaddrinfo(addrinfo&, const char*, const char*, int = 0, int = 0, int = 0, int = 0);
addrinfo& getaddrinfo(addrinfo&, const char*, const char*, int, int, int, int, std::error_code&) noexcept;
addrinfo getaddrinfo(const char*, const char*, int = 0, int = 0, int = 0, int = 0);
addrinfo getaddrinfo(const char*, const char*, int, int, int, int, std::error_code&) noexcept;

}
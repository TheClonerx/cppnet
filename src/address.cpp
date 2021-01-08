#include <cppnet/address.hpp>
#include <cppnet/getaddrinfo.hpp>

#ifdef _WIN32
#define ERROR_NUMBER WSAGetLastError()
#else
#define ERROR_NUMBER errno
#endif

net::address net::address::from_ipv4(std::string_view host, std::uint16_t port)
{
    sockaddr_in addr {};
    constexpr size_t len = sizeof(addr);

    addr.sin_family = AF_INET;
    int result = inet_pton(AF_INET, host.data(), &addr.sin_addr);
    if (result == 0)
        throw std::invalid_argument("the address was not parseable in the specified address family");
    else if (result == -1)
        throw std::system_error(ERROR_NUMBER, std::system_category());
    addr.sin_port = htons(port);

    address ret;
    std::memcpy(&ret.m_socket_address, &addr, len);
    ret.m_socket_address_size = len;
    return ret;
}

net::address net::address::from_ipv4(net::any_addr_t, std::uint16_t port) noexcept
{
    sockaddr_in addr {};
    constexpr size_t len = sizeof(addr);

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    address ret;
    std::memcpy(&ret.m_socket_address, &addr, len);
    ret.m_socket_address_size = len;
    return ret;
}

net::address net::address::from_ipv4(net::localhost_t, std::uint16_t port) noexcept
{
    sockaddr_in addr {};
    constexpr size_t len = sizeof(addr);

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_LOOPBACK;
    addr.sin_port = htons(port);

    address ret;
    std::memcpy(&ret.m_socket_address, &addr, len);
    ret.m_socket_address_size = len;
    return ret;
}

net::address net::address::from_ipv6(std::string_view host, std::uint16_t port, std::uint32_t flowinfo, std::uint32_t scopeid)
{
    sockaddr_in6 addr {};
    constexpr size_t len = sizeof(addr);

    addr.sin6_family = AF_INET6;
    int result = inet_pton(AF_INET6, host.data(), &addr.sin6_addr);
    if (result == 0)
        throw std::invalid_argument("the address was not parseable in the specified address family");
    else if (result == -1)
        throw std::system_error(ERROR_NUMBER, std::system_category());
    addr.sin6_port = htons(port);
    addr.sin6_flowinfo = flowinfo;
    addr.sin6_scope_id = scopeid;

    address ret;
    std::memcpy(&ret.m_socket_address, &addr, len);
    ret.m_socket_address_size = len;
    return ret;
}

net::address net::address::from_ipv6(net::any_addr_t, std::uint16_t port, std::uint32_t flowinfo, std::uint32_t scopeid) noexcept
{
    sockaddr_in6 addr {};
    constexpr size_t len = sizeof(addr);

    addr.sin6_family = AF_INET6;
    addr.sin6_addr = IN6ADDR_ANY_INIT;
    addr.sin6_port = htons(port);
    addr.sin6_flowinfo = flowinfo;
    addr.sin6_scope_id = scopeid;

    address ret;
    std::memcpy(&ret.m_socket_address, &addr, len);
    ret.m_socket_address_size = len;
    return ret;
}

net::address net::address::from_ipv6(net::localhost_t, std::uint16_t port, std::uint32_t flowinfo, std::uint32_t scopeid) noexcept
{
    sockaddr_in6 addr {};
    constexpr size_t len = sizeof(addr);

    addr.sin6_family = AF_INET6;
    addr.sin6_addr = IN6ADDR_LOOPBACK_INIT;
    addr.sin6_port = htons(port);
    addr.sin6_flowinfo = flowinfo;
    addr.sin6_scope_id = scopeid;

    address ret;
    std::memcpy(&ret.m_socket_address, &addr, len);
    ret.m_socket_address_size = len;
    return ret;
}

#ifndef _WIN32
net::address net::address::from_unix(std::string_view path)
{
    sockaddr_un addr {};
    constexpr size_t len = sizeof(addr);

    addr.sun_family = AF_UNIX;
    if (path.size() > sizeof(addr.sun_path) - 1)
        throw std::length_error("Path is too long");
    else // zero size paths are unnamed, std::memcpy will do nothing in this case
        std::memcpy(addr.sun_path, path.data(), path.size());

    address ret;
    std::memcpy(&ret.m_socket_address, &addr, len);
    ret.m_socket_address_size = len;
    return ret;
}
#endif

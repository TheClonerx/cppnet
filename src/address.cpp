#include <net/address.hpp>
#include <net/getaddrinfo.hpp>


net::address net::address::from_ipv4(std::string_view host, uint16_t port)
{
    sockaddr_in addr {};
    constexpr size_t len = sizeof(addr);

    addr.sin_family = AF_INET;
    int success = inet_pton(AF_INET, host.data(), &addr.sin_addr);
    if (!success)
        throw std::runtime_error("Error parsing dotted ipv4 address");
    addr.sin_port = htons(port);

    address ret;
    std::memcpy(&ret.m_socket_address, &addr, len);
    ret.m_socket_address_size = len;
    return ret;
}

net::address net::address::from_ipv4(net::any_addr_t, uint16_t port) noexcept
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

net::address net::address::from_ipv4(net::localhost_t, uint16_t port) noexcept
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

net::address net::address::from_ipv6(std::string_view host, uint16_t port, uint32_t flowinfo, uint32_t scopeid)
{
    sockaddr_in6 addr {};
    constexpr size_t len = sizeof(addr);

    addr.sin6_family = AF_INET6;
    int success = inet_pton(AF_INET6, host.data(), &addr.sin6_addr);
    if (!success)
        throw std::runtime_error("Error parsing hex ipv6 address");
    addr.sin6_port = htons(port);
    addr.sin6_flowinfo = flowinfo;
    addr.sin6_scope_id = scopeid;

    address ret;
    std::memcpy(&ret.m_socket_address, &addr, len);
    ret.m_socket_address_size = len;
    return ret;
}

net::address net::address::from_ipv6(net::any_addr_t, uint16_t port, uint32_t flowinfo, uint32_t scopeid) noexcept
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

net::address net::address::from_ipv6(net::localhost_t, uint16_t port, uint32_t flowinfo, uint32_t scopeid) noexcept
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
    if (path.size() > sizeof(addr.sun_path))
        throw std::length_error("Path is too long");
    else if (path.size() && path.back() != '\0') // std::string_view doesn't need to be nul terminated
        throw std::runtime_error("Path must be null terminated");
    else // zero size paths are unnamed, std::memcpy will do nothing in this case
        std::memcpy(addr.sun_path, path.data(), path.size());

    address ret;
    std::memcpy(&ret.m_socket_address, &addr, len);
    ret.m_socket_address_size = len;
    return ret;
}
#endif
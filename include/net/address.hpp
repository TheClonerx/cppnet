#pragma once

#include <cstring> // std::memcpy
#include <stdexcept> // class std::runtime_error
#include <string_view> // class std::string_view

#ifdef _WIN32
#include <WinSock2.h>
#else
#include <arpa/inet.h> // inet_pton
#include <netdb.h> // struct sockaddr_in
#include <sys/socket.h> // struct sockaddr_storage
#include <sys/un.h> // struct sockaddr_un
#endif

namespace net {

class addrinfo;

constexpr struct any_addr_t {
} any_addr;

constexpr struct localhost_t {
} localhost;

class address {

public:
    constexpr static uint16_t invalid_family = -1;
    friend class socket;

    constexpr address() noexcept
        : m_socket_address { invalid_family }
        , m_socket_address_size { sizeof(m_socket_address) }
    {
    }

    constexpr address(const address&) noexcept = default;
    constexpr address(address&&) noexcept = default;

    constexpr address& operator=(const address&) noexcept = default;
    constexpr address& operator=(address&&) noexcept = default;

    address(sockaddr_storage addr, size_t len)
        : m_socket_address { addr }
        , m_socket_address_size { len }
    {
    }

private:
    inline static sockaddr_storage sockaddr_storage_from_sockaddr_and_size(const sockaddr* addr, size_t size)
    {
        sockaddr_storage ret {};
        std::memcpy(&ret, addr, size);
        return ret;
    }

public:
    inline address(const sockaddr* addr, size_t len)
        : m_socket_address { sockaddr_storage_from_sockaddr_and_size(addr, len) }
        , m_socket_address_size { len }
    {
    }

    static address from_ipv4(std::string_view host, uint16_t port);
    static address from_ipv4(any_addr_t, uint16_t port) noexcept;
    static address from_ipv4(localhost_t, uint16_t port) noexcept;

    static address from_ipv6(std::string_view host, uint16_t port, uint32_t flowinfo, uint32_t scopeid);
    static address from_ipv6(any_addr_t, uint16_t port, uint32_t flowinfo, uint32_t scopeid) noexcept;
    static address from_ipv6(localhost_t, uint16_t port, uint32_t flowinfo, uint32_t scopeid) noexcept;

#ifndef _WIN32
    static address from_unix(std::string_view path);
#endif

    constexpr uint16_t family() const noexcept
    {
        return m_socket_address.ss_family;
    }

    inline const sockaddr* address_pointer() const noexcept
    {
        return reinterpret_cast<const sockaddr*>(&m_socket_address);
    }

    constexpr std::size_t address_size() const noexcept
    {
        return m_socket_address_size;
    }

private:
    sockaddr_storage m_socket_address;
    std::size_t m_socket_address_size;
};

} // namespace net

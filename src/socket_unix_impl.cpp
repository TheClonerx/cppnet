#include "net/socket.hpp"

#include <fcntl.h>
#include <unistd.h>

#define THROW_ERRNO throw std::system_error(errno, std::system_category())
#define ASSIGN_ERRNO(e) e.assign(errno, std::system_category())
#define ASSIGN_ZERO(e) e.assign(0, std::system_category())

net::socket::socket(int family, int type, int protocol)
    : m_Handle(::socket(family, type, protocol))
{
    if (m_Handle == invalid_handle)
        THROW_ERRNO;
}

net::socket::socket(int family, int type, int protocol, std::error_code& e) noexcept
    : m_Handle(::socket(family, type, protocol))
{
    if (m_Handle == invalid_handle)
        ASSIGN_ERRNO(e);
    else
        ASSIGN_ZERO(e);
}

std::pair<net::socket, net::socket> net::socket::pair(int family, int type, int protocol, std::error_code& e) noexcept
{
    native_handle_type handles[2];
    if (::socketpair(family, type, protocol, handles) < 0) {
        ASSIGN_ERRNO(e);
        return {};
    } else {
        ASSIGN_ZERO(e);
        return { net::socket { from_native_handle, handles[0] }, net::socket { from_native_handle, handles[1] } };
    }
}

net::socket net::socket::dup(std::error_code& e) const noexcept
{
    native_handle_type handle = ::dup(m_Handle);
    if (handle == invalid_handle)
        ASSIGN_ERRNO(e);
    else
        ASSIGN_ZERO(e);
    return { from_native_handle, handle };
}

size_t net::socket::recv(void* buffer, size_t size, int flags, std::error_code& e) noexcept
{
    ssize_t received = ::recv(m_Handle, buffer, size, flags);
    if (received < 0)
        ASSIGN_ERRNO(e);
    else
        ASSIGN_ZERO(e);
    return received;
}

size_t net::socket::recvfrom(void* buffer, size_t size, int flags, sockaddr* addr, size_t* addrlen, std::error_code& e) noexcept
{
    socklen_t len = addrlen ? *addrlen : 0;
    ssize_t received = ::recvfrom(m_Handle, buffer, size, flags, addr, addrlen ? &len : nullptr);
    if (received < 0)
        ASSIGN_ERRNO(e);
    else {
        ASSIGN_ZERO(e);
        if (addrlen)
            *addrlen = len;
    }
    return received;
}

size_t net::socket::send(const void* buffer, size_t size, int flags, std::error_code& e) noexcept
{
    ssize_t sent = ::send(m_Handle, buffer, size, flags);
    if (sent < 0)
        ASSIGN_ERRNO(e);
    else
        ASSIGN_ZERO(e);
    return sent;
}

size_t net::socket::sendto(const void* buffer, size_t size, int flags, const sockaddr* addr, size_t addrlen, std::error_code& e) noexcept
{
    ssize_t sent = ::sendto(m_Handle, buffer, size, flags, addr, addrlen);
    if (sent < 0)
        ASSIGN_ERRNO(e);
    else
        ASSIGN_ZERO(e);
    return sent;
}

void net::socket::connect(const sockaddr* addr, size_t addrlen, std::error_code& e) noexcept
{
    if (::connect(m_Handle, addr, addrlen) < 0)
        ASSIGN_ERRNO(e);
    else
        ASSIGN_ZERO(e);
}

net::socket net::socket::accept(sockaddr* addr, size_t* addrlen, std::error_code& e) noexcept
{
    socklen_t len = addrlen ? *addrlen : 0;
    native_handle_type handle = ::accept(m_Handle, addr, addrlen ? &len : nullptr);
    if (handle == invalid_handle)
        ASSIGN_ERRNO(e);
    else {
        ASSIGN_ZERO(e);
        if (addrlen)
            *addrlen = len;
    }
    return { from_native_handle, handle };
}

void net::socket::listen(int backlog, std::error_code& e) noexcept
{
    if (::listen(m_Handle, backlog) < 0)
        ASSIGN_ERRNO(e);
    else
        ASSIGN_ZERO(e);
}

void net::socket::bind(const sockaddr* addr, size_t addrlen, std::error_code& e) noexcept
{
    if (::bind(m_Handle, addr, addrlen) < 0)
        ASSIGN_ERRNO(e);
    else
        ASSIGN_ZERO(e);
}

void net::socket::getsockopt(int level, int optname, void* optval, size_t* optlen, std::error_code& e) const noexcept
{
    socklen_t len = optlen ? *optlen : 0;
    if (::getsockopt(m_Handle, level, optname, optval, optlen ? &len : nullptr) < 0)
        ASSIGN_ERRNO(e);
    else {
        ASSIGN_ZERO(e);
        if (optlen)
            *optlen = len;
    }
}

void net::socket::setsockopt(int level, int optname, const void* optval, size_t optlen, std::error_code& e) noexcept
{
    if (::setsockopt(m_Handle, level, optname, optval, optlen) < 0)
        ASSIGN_ERRNO(e);
    else
        ASSIGN_ZERO(e);
}

void net::socket::setblocking(bool block, std::error_code& e) noexcept
{
    int r = fcntl(m_Handle, F_GETFL);
    if (r < 0) {
        ASSIGN_ERRNO(e);
        return;
    }
    int new_flags;
    if (block)
        new_flags = r & (~O_NONBLOCK);
    else
        new_flags = r | O_NONBLOCK;
    if (new_flags != r)
        r = fcntl(m_Handle, F_SETFL, new_flags);
    if (r < 0)
        ASSIGN_ERRNO(e);
    else
        ASSIGN_ZERO(e);
}

int net::socket::family(std::error_code& e) const noexcept
{
    return getsockopt<int>(SOL_SOCKET, SO_DOMAIN, e);
}

int net::socket::protocol(std::error_code& e) const noexcept
{
    return getsockopt<int>(SOL_SOCKET, SO_PROTOCOL, e);
}

void net::socket::shutdown(int how, std::error_code& e) noexcept
{
    if (::shutdown(m_Handle, how) < 0)
        ASSIGN_ERRNO(e);
    else
        ASSIGN_ZERO(e);
}

void net::socket::close(std::error_code& e) noexcept
{
    if (::close(m_Handle) < 0)
        ASSIGN_ERRNO(e);
    else {
        ASSIGN_ZERO(e);
        m_Handle = invalid_handle;
    }
}

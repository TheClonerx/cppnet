#include <cppnet/socket.hpp>

#define THROW_IF_ERROR(e) \
    if (e)                \
    throw std::system_error(e)
#define RETURN_IF_ERROR(e) \
    if (e)                 \
    return

/*
 * IMPORTANT!!
 * Don't use `errno` here.
 * Windows errors are not set properly to it.
 * Use errno in socket_unix_impl.cpp and WSAGetLastError() in socket_win32_impl.cpp
 */

net::socket::~socket() noexcept
{
    if (m_handle != invalid_handle)
        close();
}

std::pair<net::socket, net::socket> net::socket::pair(int family, int type, int protocol)
{
    std::error_code e;
    auto socket_pair = pair(family, type, protocol, e);
    THROW_IF_ERROR(e);
    return socket_pair;
}

net::socket net::socket::dup() const
{
    std::error_code e;
    auto sock = dup(e);
    THROW_IF_ERROR(e);
    return sock;
}

size_t net::socket::recv(void* buffer, size_t size, int flags)
{
    std::error_code e;
    size_t received = recv(buffer, size, flags, e);
    THROW_IF_ERROR(e);
    return received;
}

size_t net::socket::recvfrom(void* buffer, size_t size, int flags, sockaddr* addr, size_t* len)
{
    std::error_code e;
    size_t received = recvfrom(buffer, size, flags, addr, len, e);
    THROW_IF_ERROR(e);
    return received;
}

size_t net::socket::send(const void* buffer, size_t size, int flags)
{
    std::error_code e;
    size_t sent = send(buffer, size, flags, e);
    THROW_IF_ERROR(e);
    return sent;
}


size_t net::socket::sendto(const void* buffer, size_t size, int flags, const sockaddr* addr, size_t addrlen)
{
    std::error_code e;
    size_t sent = sendto(buffer, size, flags, addr, addrlen, e);
    THROW_IF_ERROR(e);
    return sent;
}

void net::socket::connect(const sockaddr* addr, size_t len)
{
    std::error_code e;
    connect(addr, len, e);
    THROW_IF_ERROR(e);
}

net::socket net::socket::accept(sockaddr* addr, size_t* len)
{
    std::error_code e;
    net::socket sock = accept(addr, len, e);
    THROW_IF_ERROR(e);
    return sock;
}

void net::socket::listen(int backlog)
{
    std::error_code e;
    listen(backlog, e);
    THROW_IF_ERROR(e);
}

void net::socket::bind(const sockaddr* addr, size_t addrlen)
{
    std::error_code e;
    bind(addr, addrlen, e);
    THROW_IF_ERROR(e);
}

void net::socket::getsockopt(int level, int optname, void* optval, size_t* optlen) const
{
    std::error_code e;
    getsockopt(level, optname, optval, optlen, e);
    THROW_IF_ERROR(e);
}

void net::socket::setsockopt(int level, int optname, const void* optval, size_t optlen)
{
    std::error_code e;
    setsockopt(level, optname, optval, optlen, e);
    THROW_IF_ERROR(e);
}

void net::socket::setblocking(bool block)
{
    std::error_code e;
    setblocking(block, e);
    THROW_IF_ERROR(e);
}

int net::socket::family() const
{
    std::error_code e;
    int f = family(e);
    THROW_IF_ERROR(e);
    return f;
}

int net::socket::type(std::error_code& e) const noexcept
{
    return getsockopt<int>(SOL_SOCKET, SO_TYPE, e);
}

int net::socket::type() const
{
    return getsockopt<int>(SOL_SOCKET, SO_TYPE);
}

int net::socket::protocol() const
{
    std::error_code e;
    int p = protocol(e);
    THROW_IF_ERROR(e);
    return p;
}

bool net::socket::accepting() const
{
    return getsockopt<int>(SOL_SOCKET, SO_ACCEPTCONN);
}

bool net::socket::accepting(std::error_code& e) const noexcept
{
    return getsockopt<int>(SOL_SOCKET, SO_ACCEPTCONN, e);
}

void net::socket::shutdown(int how)
{
    std::error_code e;
    shutdown(how, e);
    THROW_IF_ERROR(e);
}

void net::socket::close()
{
    std::error_code e;
    close(e);
    THROW_IF_ERROR(e);
}

net::address net::socket::getsockname()
{
    std::error_code e;
    net::address address = getsockname(e);
    THROW_IF_ERROR(e);
    return address;
}

net::address net::socket::getpeername()
{
    std::error_code e;
    net::address address = getpeername(e);
    THROW_IF_ERROR(e);
    return address;
}

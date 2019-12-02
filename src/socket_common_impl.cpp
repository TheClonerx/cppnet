#include "net/socket.hpp"

#define THROW_IF_ERROR(e) \
    if (e)                \
    throw std::system_error(e)
#define RETURN_IF_ERROR(e) \
    if (e)                 \
    return

net::socket::socket(net::socket::from_native_handle_t, net::socket::native_handle_type handle)
    : m_Handle(handle)
{
}

net::socket::socket(net::socket&& rhs) noexcept
    : m_Handle(std::exchange(rhs.m_Handle, invalid_handle))
{
}

net::socket& net::socket::operator=(net::socket&& rhs) noexcept
{
    m_Handle = std::exchange(rhs.m_Handle, invalid_handle);
    return *this;
}

net::socket::~socket() noexcept
{
    if (m_Handle != invalid_handle)
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

size_t net::socket::send(std::string_view buffer, int flags, std::error_code& e) noexcept
{
    return send(buffer.data(), buffer.size(), flags, e);
}

size_t net::socket::send(std::string_view buffer, int flags)
{
    return send(buffer.data(), buffer.size(), flags);
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

net::socket::native_handle_type net::socket::native_handle() noexcept
{
    return m_Handle;
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
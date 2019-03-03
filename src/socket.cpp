#include "net/socket.hpp"
#include <cassert>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <system_error>
#include <tuple>
#include <utility>

#include <arpa/inet.h>
#include <cstdlib>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

#define THROW_ERRNO throw std::system_error(errno, std::system_category())
#define ASSIGN_ERRNO(e) e.assign(errno, std::system_category())
#define ASSIGN_ZERO(e) e.assign(0, std::system_category())
#define CHECK_FOR_THROW(r) \
    if (r < 0)             \
    THROW_ERRNO
#define CHECK_FOR_ASSIGN(r, e) \
    if (r < 0)                 \
        ASSIGN_ERRNO(e);       \
    else                       \
        ASSIGN_ZERO(e)

net::socket::socket()
    : net::socket(AF_INET, SOCK_STREAM, 0)
{
}

net::socket::socket(std::error_code& e) noexcept
    : net::socket(AF_INET, SOCK_STREAM, 0, e)
{
}

net::socket::socket(int family, int type, int proto)
    : fd(::socket(family, type, proto))
{
    CHECK_FOR_THROW(fd);
}

net::socket::socket(int family, int type, int proto, std::error_code& e) noexcept
    : fd(::socket(family, type, proto))
{
    CHECK_FOR_ASSIGN(fd, e);
}

net::socket::~socket() noexcept
{
    if (fd >= 0)
        ::close(fd); // ignore error
}

net::socket net::socket::from_fileno(int fd) noexcept
{
    // net::socket is just a wraper around an int
    return std::move(*reinterpret_cast<net::socket*>(&fd));
}

net::socket net::socket::dup() const
{
    int r = ::dup(fd);
    CHECK_FOR_THROW(r);
    return net::socket::from_fileno(r);
}

net::socket net::socket::dup(std::error_code& e) const noexcept
{
    int r = ::dup(fd);
    CHECK_FOR_ASSIGN(r, e);
    return net::socket::from_fileno(r);
}

size_t net::socket::recv(void* buff, size_t size, int flags)
{
    ssize_t ret = ::recv(fd, buff, size, flags);
    CHECK_FOR_THROW(ret);
    return ret;
}

size_t net::socket::recv(void* buff, size_t size, int flags, std::error_code& e) noexcept
{
    ssize_t ret = ::recv(fd, buff, size, flags);
    CHECK_FOR_ASSIGN(ret, e);
    return ret;
}

size_t net::socket::send(const void* buff, size_t size, int flags)
{
    ssize_t r = ::send(fd, buff, size, flags);
    CHECK_FOR_THROW(r);
    return r;
}

size_t net::socket::send(const void* buff, size_t size, int flags, std::error_code& e) noexcept
{
    ssize_t r = ::send(fd, buff, size, flags);
    CHECK_FOR_ASSIGN(r, e);
    return r;
}

size_t net::socket::send(std::string_view b, int flags)
{
    ssize_t r = ::send(fd, b.data(), b.size(), flags);
    CHECK_FOR_THROW(r);
    return r;
}

size_t net::socket::send(std::string_view b, int flags, std::error_code& e) noexcept
{
    ssize_t r = ::send(fd, b.data(), b.size(), flags);
    CHECK_FOR_ASSIGN(r, e);
    return r;
}

#include "net/getaddrinfo.hpp"

void net::socket::connect(std::string_view addr, uint16_t port)
{
    net::addrinfo ainfo = net::getaddrinfo(addr.data(), std::to_string(port).c_str(), family(), type(), protocol());
    CHECK_FOR_THROW(::connect(fd, reinterpret_cast<const sockaddr*>(&ainfo.addr), ainfo.addrlen));
}

void net::socket::connect(std::string_view addr, uint16_t port, std::error_code& e) noexcept
{
    net::addrinfo ainfo = net::getaddrinfo(addr.data(), std::to_string(port).c_str(), family(), type(), protocol(), 0, e);
    CHECK_FOR_ASSIGN(::connect(fd, reinterpret_cast<const sockaddr*>(&ainfo.addr), ainfo.addrlen), e);
}

net::socket net::socket::accept()
{
    int r = ::accept(fd, nullptr, nullptr);
    CHECK_FOR_THROW(r);
    return net::socket::from_fileno(r);
}

net::socket net::socket::accept(std::error_code& e) noexcept
{
    int r = ::accept(fd, nullptr, nullptr);
    CHECK_FOR_ASSIGN(r, e);
    return net::socket::from_fileno(r);
}

net::socket net::socket::accept(sockaddr& addr, socklen_t& socklen)
{
    int r = ::accept(fd, &addr, &socklen);
    CHECK_FOR_THROW(r);
    return net::socket::from_fileno(r);
}

net::socket net::socket::accept(sockaddr& addr, socklen_t& socklen, std::error_code& e) noexcept
{
    int r = ::accept(fd, &addr, &socklen);
    CHECK_FOR_ASSIGN(r, e);
    return net::socket::from_fileno(r);
}

#ifdef _GNU_SOURCE
net::socket net::socket::accept(int flags)
{
    int r = ::accept4(fd, nullptr, nullptr, flags);
    CHECK_FOR_THROW(r);
    return net::socket::from_fileno(r);
}

net::socket net::socket::accept(int flags, std::error_code& e) noexcept
{
    int r = ::accept4(fd, nullptr, nullptr, flags);
    CHECK_FOR_ASSIGN(r, e);
    return net::socket::from_fileno(r);
}

net::socket net::socket::accept(sockaddr& addr, socklen_t& socklen, int flags)
{
    int r = ::accept4(fd, &addr, &socklen, flags);
    CHECK_FOR_THROW(r);
    return net::socket::from_fileno(r);
}

net::socket net::socket::accept(sockaddr& addr, socklen_t& socklen, int flags, std::error_code& e) noexcept
{
    int r = ::accept4(fd, &addr, &socklen, flags);
    CHECK_FOR_ASSIGN(r, e);
    return net::socket::from_fileno(r);
}
#endif

void net::socket::listen(int backlog)
{
    CHECK_FOR_THROW(::listen(fd, backlog));
}

void net::socket::listen(int backlog, std::error_code& e) noexcept
{
    CHECK_FOR_ASSIGN(::listen(fd, backlog), e);
}

void net::socket::bind(std::string_view addr, uint16_t port)
{
    net::addrinfo ainfo = net::getaddrinfo(addr.data(), std::to_string(port).c_str(), family(), type(), protocol());
    CHECK_FOR_THROW(::bind(fd, reinterpret_cast<const sockaddr*>(&ainfo.addr), ainfo.addrlen));
}

void net::socket::bind(std::string_view addr, uint16_t port, std::error_code& e) noexcept
{
    net::addrinfo ainfo = net::getaddrinfo(addr.data(), std::to_string(port).c_str(), family(), type(), protocol(), 0, e);
    if (e)
        return;
    CHECK_FOR_ASSIGN(::bind(fd, reinterpret_cast<const sockaddr*>(&ainfo.addr), ainfo.addrlen), e);
}

void net::socket::bind(net::any_addr_t, uint16_t port)
{
    sockaddr_in sa;
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = INADDR_ANY;
    sa.sin_port = htons(port);
    CHECK_FOR_THROW(::bind(fd, reinterpret_cast<const sockaddr*>(&sa), sizeof(sa)));
}

void net::socket::bind(net::any_addr_t, uint16_t port, std::error_code& e) noexcept
{
    sockaddr_in sa;
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = INADDR_ANY;
    sa.sin_port = htons(port);
    CHECK_FOR_ASSIGN(::bind(fd, reinterpret_cast<const sockaddr*>(&sa), sizeof(sa)), e);
}

void net::socket::bind(net::localhost_t, uint16_t port)
{
    sockaddr_in sa;
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = INADDR_LOOPBACK;
    sa.sin_port = htons(port);
    CHECK_FOR_THROW(::bind(fd, reinterpret_cast<const sockaddr*>(&sa), sizeof(sa)));
}

void net::socket::bind(net::localhost_t, uint16_t port, std::error_code& e) noexcept
{
    sockaddr_in sa;
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = INADDR_LOOPBACK;
    sa.sin_port = htons(port);
    CHECK_FOR_ASSIGN(::bind(fd, reinterpret_cast<const sockaddr*>(&sa), sizeof(sa)), e);
}

void net::socket::getsockopt(int level, int optname, void* optval, socklen_t* optlen) const
{
    CHECK_FOR_THROW(::getsockopt(fd, level, optname, optval, optlen));
}

void net::socket::getsockopt(int level, int optname, void* optval, socklen_t* optlen, std::error_code& e) const noexcept
{
    CHECK_FOR_ASSIGN(::getsockopt(fd, level, optname, optval, optlen), e);
}

void net::socket::setsockopt(int level, int optname, void* optval, socklen_t optlen)
{
    CHECK_FOR_THROW(::setsockopt(fd, level, optname, optval, optlen));
}

void net::socket::setsockopt(int level, int optname, void* optval, socklen_t optlen, std::error_code& e) noexcept
{
    CHECK_FOR_ASSIGN(::setsockopt(fd, level, optname, optval, optlen), e);
}

void net::socket::setblocking(bool block)
{
    int r = fcntl(fd, F_GETFL);
    CHECK_FOR_THROW(r);
    int new_flags;
    if (block)
        new_flags = r & (~O_NONBLOCK);
    else
        new_flags = r | O_NONBLOCK;
    if (new_flags != r)
        r = fcntl(fd, F_SETFL, new_flags);
    CHECK_FOR_THROW(r);
}

void net::socket::setblocking(bool block, std::error_code& e) noexcept
{
    int r = fcntl(fd, F_GETFL);
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
        r = fcntl(fd, F_SETFL, new_flags);
    CHECK_FOR_ASSIGN(r, e);
}

int net::socket::fileno() const noexcept
{
    return fd;
}

int net::socket::family() const
{
    return getsockopt<int>(SOL_SOCKET, SO_DOMAIN);
}

int net::socket::family(std::error_code& e) const noexcept
{
    return getsockopt<int>(SOL_SOCKET, SO_DOMAIN, e);
}

int net::socket::type() const
{
    return getsockopt<int>(SOL_SOCKET, SO_TYPE);
}

int net::socket::type(std::error_code& e) const noexcept
{
    return getsockopt<int>(SOL_SOCKET, SO_TYPE, e);
}

int net::socket::protocol() const
{
    return getsockopt<int>(SOL_SOCKET, SO_PROTOCOL);
}

int net::socket::protocol(std::error_code& e) const noexcept
{
    return getsockopt<int>(SOL_SOCKET, SO_PROTOCOL, e);
}

bool net::socket::is_accepting() const
{
    return getsockopt<int>(SOL_SOCKET, SO_ACCEPTCONN);
}

bool net::socket::is_accepting(std::error_code& e) const noexcept
{
    return getsockopt<int>(SOL_SOCKET, SO_ACCEPTCONN, e);
}

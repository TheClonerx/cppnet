#include "net/socket.hpp"
#include <cassert>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <system_error>
#include <tuple>
#include <utility>

#include <cstdlib>
#include <cstdint>
#include <fcntl.h>

#ifdef _WIN32
#include <Winsock2.h>
#else
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#endif

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

#ifdef _WIN32
#define CLOSE_SOCKET closesocket
#else
#define CLOSE_SOCKET close
#endif

net::socket::socket()
    : net::socket(AF_INET, SOCK_STREAM, 0)
{
}

net::socket::socket(net::socket&& rhs) noexcept
    : fd(rhs.fd)
{
    rhs.fd = -1;
}

net::socket& net::socket::operator=(net::socket&& rhs) noexcept
{
    fd = rhs.fd;
    rhs.fd = -1;
    return *this;
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
        ::CLOSE_SOCKET(fd); // ignore error
}

net::socket net::socket::from_fileno(native_handle_type fd) noexcept
{
    // net::socket is just a wraper around an int
    return reinterpret_cast<net::socket&&>(fd);
}

std::pair<net::socket, net::socket> net::socket::pair(int family, int type, int proto)
{
	std::error_code e;
	auto ret = pair(family, type, proto, e);
	if (e)
		throw std::system_error(e);
	return ret;
}

std::pair<net::socket, net::socket> net::socket::pair(int family, int type, int proto, std::error_code& e) noexcept
{
#ifdef _WIN32
	auto sock1 = net::socket{ family, type, proto, e };
	auto sock2 = net::socket::from_fileno(INVALID_SOCKET);
	if (e) goto win_error;

	sock2 = net::socket{ family, type, proto, e };
	if (e) goto win_error;

	sock1.listen(1, e);
	if (e) goto win_error;

	sock1.bind(net::localhost, 0, e);
	if (e) goto win_error;

	SOCKADDR_STORAGE addr;
	int size = sizeof(addr);
	CHECK_FOR_ASSIGN(::getsockname(sock1.fileno(), (sockaddr*)&addr, &size), e);
	if (e) goto win_error;

	sock2.connect((const sockaddr*)&addr, size, e);
	if (e) goto win_error;

	return { std::move(sock1), std::move(sock2) };
win_error:
	return { net::socket::from_fileno(INVALID_SOCKET), net::socket::from_fileno(INVALID_SOCKET) };
#else
    int fds[2];
    if (::socketpair(family, type, proto, fds) < 0) {
        ASSIGN_ERRNO(e);
        return { net::socket::from_fileno(-1), net::socket::from_fileno(-1) };
    } else {
        ASSIGN_ZERO(e);
        return { net::socket::from_fileno(fds[0]), net::socket::from_fileno(fds[1]) };
    }
#endif
}

net::socket net::socket::dup() const
{
	std::error_code e;
	auto sock = dup(e);
	if (e)
		throw std::system_error(e);
	return sock;
}

net::socket net::socket::dup(std::error_code& e) const noexcept
{
#ifdef _WIN32
	SOCKET handle;
	// TODO: Error checking
	DuplicateHandle(GetCurrentProcess(), (HANDLE)fileno(), GetCurrentProcess(), (HANDLE*)&handle, 0, FALSE, DUPLICATE_SAME_ACCESS);
	return net::socket::from_fileno(handle);
#else
    int r = ::dup(fd);
    CHECK_FOR_ASSIGN(r, e);
    return net::socket::from_fileno(r);
#endif
}

size_t net::socket::recv(void* buff, size_t size, int flags)
{
    impl::ssize_t ret = ::recv(fd, reinterpret_cast<char*>(buff), size, flags);
    CHECK_FOR_THROW(ret);
    return ret;
}

size_t net::socket::recv(void* buff, size_t size, int flags, std::error_code& e) noexcept
{
	impl::ssize_t ret = ::recv(fd, reinterpret_cast<char*>(buff), size, flags);
    CHECK_FOR_ASSIGN(ret, e);
    return ret;
}

size_t net::socket::recvfrom(void* buff, size_t size, int flags, sockaddr* addr, impl::socklen_t* addrlen)
{
	impl::ssize_t ret = ::recvfrom(fd, reinterpret_cast<char*>(buff), size, flags, addr, addrlen);
    CHECK_FOR_THROW(ret);
    return ret;
}

size_t net::socket::recvfrom(void* buff, size_t size, int flags, sockaddr* addr, impl::socklen_t* addrlen, std::error_code& e) noexcept
{
	impl::ssize_t ret = ::recvfrom(fd, reinterpret_cast<char*>(buff), size, flags, addr, addrlen);
    CHECK_FOR_ASSIGN(ret, e);
    return ret;
}
#ifndef _WIN32
size_t net::socket::recvmsg(msghdr* msg, int flags)
{
    ssize_t ret = ::recvmsg(fd, msg, flags);
    CHECK_FOR_THROW(ret);
    return ret;
}

size_t net::socket::recvmsg(msghdr* msg, int flags, std::error_code& e) noexcept
{
    ssize_t ret = ::recvmsg(fd, msg, flags);
    CHECK_FOR_ASSIGN(ret, e);
    return ret;
}
#endif
size_t net::socket::send(const void* buff, size_t size, int flags)
{
	impl::ssize_t r = ::send(fd, reinterpret_cast<const char*>(buff), size, flags);
    CHECK_FOR_THROW(r);
    return r;
}

size_t net::socket::send(const void* buff, size_t size, int flags, std::error_code& e) noexcept
{
	impl::ssize_t r = ::send(fd, reinterpret_cast<const char*>(buff), size, flags);
    CHECK_FOR_ASSIGN(r, e);
    return r;
}

size_t net::socket::send(std::string_view b, int flags)
{
	impl::ssize_t r = ::send(fd, b.data(), b.size(), flags);
    CHECK_FOR_THROW(r);
    return r;
}

size_t net::socket::send(std::string_view b, int flags, std::error_code& e) noexcept
{
	impl::ssize_t r = ::send(fd, b.data(), b.size(), flags);
    CHECK_FOR_ASSIGN(r, e);
    return r;
}

size_t net::socket::sendto(const void* buff, size_t size, int flags, const sockaddr* addr, impl::socklen_t addrlen)
{
	impl::ssize_t r = ::sendto(fd, reinterpret_cast<const char*>(buff), size, flags, addr, addrlen);
    CHECK_FOR_THROW(r);
    return r;
}

size_t net::socket::sendto(const void* buff, size_t size, int flags, const sockaddr* addr, impl::socklen_t addrlen, std::error_code& e) noexcept
{
	impl::ssize_t r = ::sendto(fd, reinterpret_cast<const char*>(buff), size, flags, addr, addrlen);
    CHECK_FOR_ASSIGN(r, e);
    return r;
}
#ifndef _WIN32
size_t net::socket::sendmsg(const msghdr* msg, int flags)
{
    ssize_t r = ::sendmsg(fd, msg, flags);
    CHECK_FOR_THROW(r);
    return r;
}

size_t net::socket::sendmsg(const msghdr* msg, int flags, std::error_code& e) noexcept
{
    ssize_t r = ::sendmsg(fd, msg, flags);
    CHECK_FOR_ASSIGN(r, e);
    return r;
}
#endif
void net::socket::connect(const sockaddr* addr, impl::socklen_t addr_len) {
    CHECK_FOR_THROW(::connect(fd, addr, addr_len));
}

void net::socket::connect(const sockaddr* addr, impl::socklen_t addr_len, std::error_code& e) noexcept {
    CHECK_FOR_ASSIGN(::connect(fd, addr, addr_len), e);
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
	impl::socket_handle r = ::accept(fd, nullptr, nullptr);
    CHECK_FOR_THROW(r);
    return net::socket::from_fileno(r);
}

net::socket net::socket::accept(std::error_code& e) noexcept
{
	impl::socket_handle r = ::accept(fd, nullptr, nullptr);
    CHECK_FOR_ASSIGN(r, e);
    return net::socket::from_fileno(r);
}

net::socket net::socket::accept(sockaddr& addr, impl::socklen_t& socklen)
{
	impl::socket_handle r = ::accept(fd, &addr, &socklen);
    CHECK_FOR_THROW(r);
    return net::socket::from_fileno(r);
}

net::socket net::socket::accept(sockaddr& addr, impl::socklen_t& socklen, std::error_code& e) noexcept
{
    impl::socket_handle r = ::accept(fd, &addr, &socklen);
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

void net::socket::getsockopt(int level, int optname, void* optval, impl::socklen_t* optlen) const
{
    CHECK_FOR_THROW(::getsockopt(fd, level, optname, reinterpret_cast<char*>(optval), optlen));
}

void net::socket::getsockopt(int level, int optname, void* optval, impl::socklen_t* optlen, std::error_code& e) const noexcept
{
    CHECK_FOR_ASSIGN(::getsockopt(fd, level, optname, reinterpret_cast<char*>(optval), optlen), e);
}

void net::socket::setsockopt(int level, int optname, const void* optval, impl::socklen_t optlen)
{
    CHECK_FOR_THROW(::setsockopt(fd, level, optname, reinterpret_cast<const char*>(optval), optlen));
}

void net::socket::setsockopt(int level, int optname, const void* optval, impl::socklen_t optlen, std::error_code& e) noexcept
{
    CHECK_FOR_ASSIGN(::setsockopt(fd, level, optname, reinterpret_cast<const char*>(optval), optlen), e);
}

void net::socket::setblocking(bool block)
{
	std::error_code e;
	setblocking(block, e);
	if (e) throw std::system_error(e);
}

void net::socket::setblocking(bool block, std::error_code& e) noexcept
{
#ifdef _WIN32
	u_long arg = 1;
	CHECK_FOR_ASSIGN(ioctlsocket(fd, FIONBIO, &arg), e);
#else
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
#endif
}

net::socket::native_handle_type net::socket::fileno() const noexcept
{
    return fd;
}

int net::socket::family() const
{
#ifdef _WIN32
	return getsockopt<WSAPROTOCOL_INFOW>(SOL_SOCKET, SO_PROTOCOL_INFOW).iAddressFamily;
#else
	return getsockopt<int>(SOL_SOCKET, SO_DOMAIN);
#endif
    
}

int net::socket::family(std::error_code& e) const noexcept
{
#ifdef _WIN32
	return getsockopt<WSAPROTOCOL_INFOW>(SOL_SOCKET, SO_PROTOCOL_INFOW, e).iAddressFamily;
#else
	return getsockopt<int>(SOL_SOCKET, SO_DOMAIN, e);
#endif
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
#ifdef _WIN32
	return getsockopt<WSAPROTOCOL_INFOW>(SOL_SOCKET, SO_PROTOCOL_INFOW).iProtocol;
#else
    return getsockopt<int>(SOL_SOCKET, SO_PROTOCOL);
#endif
}

int net::socket::protocol(std::error_code& e) const noexcept
{
#ifdef _WIN32
	return getsockopt<WSAPROTOCOL_INFOW>(SOL_SOCKET, SO_PROTOCOL_INFOW, e).iProtocol;
#else
    return getsockopt<int>(SOL_SOCKET, SO_PROTOCOL, e);
#endif
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
    CHECK_FOR_THROW(::shutdown(fd, how));
}

void net::socket::shutdown(int how, std::error_code& e) noexcept
{
    CHECK_FOR_ASSIGN(::shutdown(fd, how), e);
}

void net::socket::close()
{
    CHECK_FOR_THROW(::CLOSE_SOCKET(fd));
    fd = -1;
}

void net::socket::close(std::error_code& e) noexcept
{
    if (::CLOSE_SOCKET(fd) < 0)
        ASSIGN_ERRNO(e);
    else {
        ASSIGN_ZERO(e);
        fd = -1;
    }
}

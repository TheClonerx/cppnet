#include "net/socket.hpp"

#define THROW_LAST_ERROR throw std::system_error(WSAGetLastError(), std::system_category())
#define ASSIGN_LAST_ERROR(e) e.assign(WSAGetLastError(), std::system_category())
#define ASSIGN_ZERO(e) e.assign(0, std::system_category())

net::socket::socket(int af, int type, int proto)
    : m_Handle(::socket(af, type, proto))
{
    if (m_Handle == invalid_handle)
        THROW_LAST_ERROR;
}

// Totally not a real socket pair, but it's the most similar thing
std::pair<net::socket, net::socket> net::socket::pair(int af, int type, int proto, std::error_code& e) noexcept
{
    net::socket sock1;
    net::socket sock2;

#define RETURN_IF_ERROR \
    if (e)              \
        return {};

    sock1 = { af, type, proto, e };
    RETURN_IF_ERROR;

    sock2 = { af, type, proto, e };
    RETURN_IF_ERROR;

    if (af == AF_INET)
        sock1.bind(net::address::from_ipv4(net::localhost, 0), e);
    else if (af == AF_INET6) // maybe getaddrinfo localhost?
        sock1.bind(net::address::from_ipv6(net::localhost, 0, 0, 0), e);
    else
        e.assign(WSAEAFNOSUPPORT, std::system_category());
    RETURN_IF_ERROR;

    if (type == SOCK_STREAM) {
        sock1.listen(1, e);
        RETURN_IF_ERROR;
    }

    net::address address = sock1.getsockname(e);
    RETURN_IF_ERROR;

    sock2.connect(address, e);
    RETURN_IF_ERROR;

    return { std::move(sock1), std::move(sock2) };

#undef RETURN_IF_ERROR
}

net::socket net::socket::dup(std::error_code& e) const noexcept
{
    auto protocol_info = getsockopt<WSAPROTOCOL_INFOW>(SOL_SOCKET, SO_PROTOCOL_INFOW);
    // WSASocketA is deprecated
    auto handle = WSASocketW(family(), type(), protocol(), &protocol_info, 0, WSA_FLAG_OVERLAPPED);
    if (handle == invalid_handle) {
        ASSIGN_LAST_ERROR(e);
        return {};
    }
    return { from_native_handle, handle };
}

int net::socket::family(std::error_code& e) const noexcept
{
    return getsockopt<WSAPROTOCOL_INFOW>(SOL_SOCKET, SO_PROTOCOL_INFOW, e).iAddressFamily;
}

net::address net::socket::getsockname(std::error_code& e) noexcept
{
    sockaddr_storage addr;
    int addr_len;
    int ret = ::getsockname(m_Handle, reinterpret_cast<sockaddr*>(&addr), &addr_len);
    if (ret < 0) {
        ASSIGN_LAST_ERROR(e);
        return {};
    } else
        return { addr, static_cast<size_t>(addr_len) };
}

net::address net::socket::getpeername(std::error_code& e) noexcept
{
    sockaddr_storage addr;
    int addr_len;
    int ret = ::getpeername(m_Handle, reinterpret_cast<sockaddr*>(&addr), &addr_len);
    if (ret < 0) {
        ASSIGN_LAST_ERROR(e);
        return {};
    } else
        return { addr, static_cast<size_t>(addr_len) };
}

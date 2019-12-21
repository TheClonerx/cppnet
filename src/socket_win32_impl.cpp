#include "net/socket.hpp"

#define THROW_LAST_ERROR throw std::system_error(WSAGetLastError(), std::system_category())
#define ASSIGN_LAST_ERROR(e) e.assign(WSAGetLastError(), std::system_category())
#define ASSIGN_ZERO(e) e.assign(0, std::system_category())

int net::socket::family(std::error_code& e) const noexcept
{
    return getsockopt<WSAPROTOCOL_INFOW>(SOL_SOCKET, SO_PROTOCOL_INFOW, e).iAddressFamily;
}

net::address net::socket::getsockname(std::error_code& e) noexcept
{
    sockaddr_storage addr;
    int addr_len;
    int ret = ::getsockname(m_Handle, reinterpret_cast<sockaddr*>(&addr), &addr_len);
    if (ret < 0)
        ASSIGN_LAST_ERROR(e);
    else
        return { addr, addr_len };
}

net::address net::socket::getpeername(std::error_code& e) noexcept
{
    sockaddr_storage addr;
    int addr_len;
    int ret = ::getpeername(m_Handle, reinterpret_cast<sockaddr*>(&addr), &addr_len);
    if (ret < 0)
        ASSIGN_LAST_ERROR(e);
    else
        return { addr, addr_len };
}

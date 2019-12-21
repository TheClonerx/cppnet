#include "net/socket.hpp"

#define THROW_LAST_ERROR throw std::system_error(WSAGetLastError(), std::system_category())
#define ASSIGN_LAST_ERROR(e) e.assign(WSAGetLastError(), std::system_category())
#define ASSIGN_ZERO(e) e.assign(0, std::system_category())

int net::socket::family(std::error_code& e) const noexcept
{
    return getsockopt<WSAPROTOCOL_INFOW>(SOL_SOCKET, SO_PROTOCOL_INFOW, e), iAddressFamily;
}

int net::socket::type(std::error_code& e) const noexcept
{
    return getsockopt<WSAPROTOCOL_INFOW>(SOL_SOCKET, SO_PROTOCOL_INFOW, e).iProtocol;
}

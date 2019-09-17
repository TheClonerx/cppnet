#include "net/socket.hpp"

int net::socket::family(std::error_code& e) const noexcept
{
    return getsockopt<WSAPROTOCOL_INFOW>(SOL_SOCKET, SO_PROTOCOL_INFOW, e), iAddressFamily;
}

int net::socket::type(std::error_code& e) const noexcept
{
    return getsockopt<WSAPROTOCOL_INFOW>(SOL_SOCKET, SO_PROTOCOL_INFOW, e).iProtocol;
}

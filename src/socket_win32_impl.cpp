#include <cppnet/socket.hpp>

#define THROW_LAST_ERROR throw std::system_error(WSAGetLastError(), std::system_category())
#define ASSIGN_LAST_ERROR(e) e.assign(WSAGetLastError(), std::system_category())
#define ASSIGN_ZERO(e) e.assign(0, std::system_category())

net::socket::socket(int af, int type, int proto)
    : m_Handle(::socket(af, type, proto))
{
    if (m_Handle == invalid_handle)
        THROW_LAST_ERROR;
}

net::socket::socket(int af, int type, int proto, std::error_code& e) noexcept
    : m_Handle(::socket(af, type, proto))
{
    if (m_Handle == invalid_handle)
        ASSIGN_LAST_ERROR(e);
    else
        ASSIGN_ZERO(e);
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

    ASSIGN_ZERO(e);
    return { std::move(sock1), std::move(sock2) };

#undef RETURN_IF_ERROR
}

net::socket net::socket::dup(std::error_code& e) const noexcept
{
    auto protocol_info = getsockopt<WSAPROTOCOL_INFOW>(SOL_SOCKET, SO_PROTOCOL_INFOW);
    // WSASocketA is deprecated
    auto handle = WSASocketW(family(), type(), protocol(), &protocol_info, 0, 0);
    if (handle == invalid_handle) {
        ASSIGN_LAST_ERROR(e);
        return {};
    }
    ASSIGN_ZERO(e);
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
    } else {
        ASSIGN_ZERO(e);
        return { addr, static_cast<size_t>(addr_len) };
    }
}

net::address net::socket::getpeername(std::error_code& e) noexcept
{
    sockaddr_storage addr;
    int addr_len;
    int ret = ::getpeername(m_Handle, reinterpret_cast<sockaddr*>(&addr), &addr_len);
    if (ret < 0) {
        ASSIGN_LAST_ERROR(e);
        return {};
    } else {
        ASSIGN_ZERO(e);
        return { addr, static_cast<size_t>(addr_len) };
    }
}

size_t net::socket::recv(void* buf, size_t len, int flags_, std::error_code& e) noexcept
{
    WSABUF buffer { static_cast<ULONG>(len), static_cast<CHAR*>(buf) };
    DWORD numberOfBytesRecvd = 0;
    DWORD flags = flags_;

    int ret = ::WSARecv(m_Handle, &buffer, 1, &numberOfBytesRecvd, &flags, nullptr, nullptr);
    if (ret < 0)
        ASSIGN_LAST_ERROR(e);
    else
        ASSIGN_ZERO(e);

    return numberOfBytesRecvd;
}

size_t net::socket::recvfrom(void* buf, size_t len, int flags_, sockaddr* from, size_t* addr_len, std::error_code& e) noexcept
{
    WSABUF buffer { static_cast<ULONG>(len), static_cast<CHAR*>(buf) };
    DWORD numberOfBytesRecvd = 0;
    DWORD flags = flags_;
    INT fromLen;
    int ret = WSARecvFrom(m_Handle, &buffer, 1, &numberOfBytesRecvd, &flags, from, &fromLen, nullptr, nullptr);
    if (ret < 0)
        ASSIGN_LAST_ERROR(e);
    else {
        *addr_len = fromLen;
        ASSIGN_ZERO(e);
    }

    return numberOfBytesRecvd;
}

size_t net::socket::send(const void* buf, size_t len, int flags, std::error_code& e) noexcept
{
    WSABUF buffer { static_cast<ULONG>(len), reinterpret_cast<CHAR*>(const_cast<void*>(buf)) };
    DWORD numberOfBytesSend = 0;
    int ret = WSASend(m_Handle, &buffer, 1, &numberOfBytesSend, flags, nullptr, nullptr);
    if (ret < 0)
        ASSIGN_LAST_ERROR(e);
    else
        ASSIGN_ZERO(e);

    return numberOfBytesSend;
}

size_t net::socket::sendto(const void* buf, size_t len, int flags, const sockaddr* addr, size_t addr_len, std::error_code& e) noexcept
{
    WSABUF buffer { static_cast<ULONG>(len), reinterpret_cast<CHAR*>(const_cast<void*>(buf)) };
    DWORD numberOfBytesSend = 0;
    int ret = WSASendTo(m_Handle, &buffer, 1, &numberOfBytesSend, flags, addr, static_cast<int>(addr_len), nullptr, nullptr);
    if (ret < 0)
        ASSIGN_LAST_ERROR(e);
    else
        ASSIGN_ZERO(e);

    return numberOfBytesSend;
}

void net::socket::connect(const sockaddr* addr, size_t addr_len, std::error_code& e) noexcept
{
    int ret = ::connect(m_Handle, addr, static_cast<int>(addr_len));
    if (ret < 0)
        ASSIGN_LAST_ERROR(e);
    else
        ASSIGN_ZERO(e);
}

net::socket net::socket::accept(sockaddr* addr, size_t* addr_len, std::error_code& e) noexcept
{
    int len;
    SOCKET new_handle = ::accept(m_Handle, addr, &len);
    if (new_handle == invalid_handle)
        ASSIGN_LAST_ERROR(e);
    else {
        *addr_len = len;
        ASSIGN_ZERO(e);
    }
    return { from_native_handle, new_handle };
}

void net::socket::listen(int backlog, std::error_code& e) noexcept
{
    int ret = ::listen(m_Handle, backlog);
    if (ret < 0)
        ASSIGN_LAST_ERROR(e);
    else
        ASSIGN_ZERO(e);
}

void net::socket::bind(const sockaddr* addr, size_t addr_len, std::error_code& e) noexcept
{
    int ret = ::bind(m_Handle, addr, static_cast<int>(addr_len));
    if (ret < 0)
        ASSIGN_LAST_ERROR(e);
    else
        ASSIGN_ZERO(e);
}

void net::socket::getsockopt(int level, int name, void* buf, size_t* len_, std::error_code& e) const noexcept
{
    int len;
    int ret = ::getsockopt(m_Handle, level, name, static_cast<char*>(buf), &len);
    if (ret < 0)
        ASSIGN_LAST_ERROR(e);
    else {
        *len_ = len;
        ASSIGN_ZERO(e);
    }
}

void net::socket::setsockopt(int level, int name, const void* buf, size_t len, std::error_code& e) noexcept
{
    int ret = ::setsockopt(m_Handle, level, name, static_cast<const char*>(buf), static_cast<int>(len));
    if (ret < 0)
        ASSIGN_LAST_ERROR(e);
    else
        ASSIGN_ZERO(e);
}

int net::socket::protocol(std::error_code& e) const noexcept
{
    return getsockopt<WSAPROTOCOL_INFOW>(SOL_SOCKET, SO_PROTOCOL_INFOW, e).iProtocol;
}

void net::socket::setblocking(bool blocks, std::error_code& e) noexcept
{
    u_long b = blocks;
    int ret = ::ioctlsocket(m_Handle, FIONBIO, &b);
    if (ret < 0)
        ASSIGN_LAST_ERROR(e);
    else
        ASSIGN_ZERO(e);
}

void net::socket::close(std::error_code& e) noexcept
{
    if (m_Handle != invalid_handle) {
        int ret = ::closesocket(m_Handle);
        if (ret < 0)
            ASSIGN_LAST_ERROR(e);
    }
    ASSIGN_ZERO(e);
}

void net::socket::shutdown(int way, std::error_code& e) noexcept
{
    int ret = ::shutdown(m_Handle, way);
    if (ret < 0)
        ASSIGN_LAST_ERROR(e);
    else
        ASSIGN_ZERO(e);
}

namespace net {
namespace impl {

    class WsaInit {
        static WSADATA wsa_data;
        static bool wsa_inited;

    public:
        WsaInit();
        ~WsaInit();
    };

} // namespace impl

} // namespace net

WSADATA net::impl::WsaInit::wsa_data;
bool net::impl::WsaInit::wsa_inited = false;

net::impl::WsaInit::WsaInit()
{
    if (!wsa_inited) {
        int error = WSAStartup(MAKEWORD(2, 2), &wsa_data);
        if (error)
            throw std::system_error(WSAGetLastError(), std::system_category());
        wsa_inited = true;
    }
}

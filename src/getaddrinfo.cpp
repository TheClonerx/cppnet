#include <cppnet/getaddrinfo.hpp>
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <netdb.h>
#include <sys/socket.h>
#endif
#include <sys/types.h>

namespace {
class addrinfo_error_category_t : public std::error_category {
public:
    char const* name() const noexcept override
    {
        return "addrinfo";
    }

    std::string message(int ecode) const override
    {
#ifdef _WIN32
        return gai_strerrorA(ecode);
#else
        return gai_strerror(ecode);
#endif
    }
};
}

static addrinfo_error_category_t const addrinfo_error_category {};

std::error_category const& net::addrinfo_category() noexcept
{
    return addrinfo_error_category;
}

net::address_info::address_info(int /*family*/, int type, int protocol, sockaddr const* addr, size_t len, char const* canonname) noexcept
    : m_type { type }
    , m_protocol { protocol }
    , m_address { addr, len }
    , m_canonname { canonname ? canonname : std::string() }
{
}

int net::address_info::family() const noexcept
{
    return m_address.family();
}

int net::address_info::type() const noexcept
{
    return m_type;
}

int net::address_info::protocol() const noexcept
{
    return m_protocol;
}

net::address const& net::address_info::address() const noexcept
{
    return m_address;
}

std::string_view net::address_info::canon_name() const noexcept
{
    return m_canonname;
}

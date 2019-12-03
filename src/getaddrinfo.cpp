#include "net/getaddrinfo.hpp"
#ifdef _WIN32
#include <WinSock2.h>
#include <ws2tcpip.h>
#else
#include <netdb.h>
#include <sys/socket.h>
#endif
#include <sys/types.h>

const std::error_category& net::addrinfo_category() noexcept
{
    class addrinfo_category_t : public std::error_category {
        // using std::error_category::error_category;
    public:
        const char* name() const noexcept override
        {
            static const char val[] = "addrinfo";
            return val;
        }
        std::string message(int ecode) const override
        {
#ifdef _WIN32
            return gai_strerrorA(ecode);
#else
            return gai_strerror(ecode);
#endif
        }
        bool equivalent(int ecode, std::error_condition const& econd) const noexcept override
        {
            return econd.category() == *this && ecode == econd.value();
        }
        bool equivalent(std::error_code const& ecode, int econd) const noexcept override
        {
            return ecode.category() == *this && ecode.value() == econd;
        }
        std::error_condition default_error_condition(int ecode) const noexcept override
        {
            return std::error_condition(ecode, *this);
        }
    };

    const static addrinfo_category_t val {};
    return val;
}

net::address_info::address_info(int /*family*/, int type, int protocol, const sockaddr* addr, size_t len, const char* canonname) noexcept
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

const net::address& net::address_info::address() const noexcept
{
    return m_address;
}

std::string_view net::address_info::canon_name() const noexcept
{
    return m_canonname;
}

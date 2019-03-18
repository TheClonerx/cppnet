#include "net/getaddrinfo.hpp"
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

const std::error_category& net::addrinfo_category()
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
            return gai_strerror(ecode);
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

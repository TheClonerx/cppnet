#pragma once
#include <chrono>
#include <system_error>
#include <utility>
#include <vector>

// if someone is going to use net::poll
// they may need some constats defined here
#include <poll.h>

namespace net {
class poll {
public:
    bool add(int fd, short eventmask);
    bool modify(int fd, short eventmask);
    bool unregister(int fd);

    size_t execute(std::chrono::milliseconds);
    size_t execute(std::chrono::milliseconds, std::error_code&) noexcept;

#ifdef _GNU_SOURCE
    // for ppoll
    size_t execute(std::chrono::nanoseconds, const sigset_t&);
    size_t execute(std::chrono::nanoseconds, const sigset_t&, std::error_code&) noexcept;
#endif

    template <typename It>
    void get(It start, It stop) const noexcept(noexcept(std::get<0>(*start) = 0) && noexcept(std::get<1>(*start) = 0) && noexcept(++start == stop))
    {
        for (const pollfd& fd : fds) {
            if (fd.revents) {
                std::get<0>(*start) = fd.fd;
                std::get<1>(*start) = fd.revents;
                if (++start == stop)
                    return;
            }
        }
    }

    template <typename It>
    void get(It start) const noexcept(noexcept(std::get<0>(*start) = 0) && noexcept(std::get<1>(*start) = 0) && noexcept(++start))
    {
        for (const pollfd& fd : fds) {
            if (fd.revents) {
                std::get<0>(*start) = fd.fd;
                std::get<1>(*start) = fd.revents;
                ++start;
            }
        }
    }

private:
    std::vector<pollfd> fds;
};
} // net

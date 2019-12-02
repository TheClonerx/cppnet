#pragma once
#include <chrono>
#include <optional>
#include <system_error>
#include <utility>
#include <vector>

#ifdef _WIN32
#include <WinSock2.h>
#else
// if someone is going to use net::poll
// they may need some constats defined here
#include <poll.h>
#endif

#include "net/socket.hpp"

namespace net {
class poll {
public:
    bool add(socket::native_handle_type fd, short events);
    bool modify(socket::native_handle_type fd, short events);
    bool remove(socket::native_handle_type fd);

    size_t execute(std::optional<std::chrono::milliseconds> timeout);
    size_t execute(std::optional<std::chrono::milliseconds> timeout, std::error_code&) noexcept;

#ifdef _GNU_SOURCE
    // for ppoll
    size_t execute(std::optional<std::chrono::nanoseconds> timeout, const sigset_t& sigmask);
    size_t execute(std::optional<std::chrono::nanoseconds> timeout, const sigset_t& sigmask, std::error_code&) noexcept;
#endif

    // ranges
    template <typename OIt>
    OIt get(OIt start, OIt stop) const noexcept(noexcept(*start = { 0, 0 }) && noexcept(++start == stop))
    {
        for (const pollfd& fd : fds) {
            if (start == stop)
                return stop;
            if (fd.revents) {
                *start = { fd.fd, fd.revents };
                ++start;
            }
        }
        return start;
    }

    // inserters
    template <typename OIt>
    OIt get(OIt it) const noexcept(noexcept(*it = { 0, 0 }) && noexcept(++it))
    {
        for (const pollfd& fd : fds) {
            if (fd.revents) {
                *it = { fd.fd, fd.revents };
                ++it;
            }
        }
        return it;
    }

private:
    std::vector<pollfd> fds;
};
} // net

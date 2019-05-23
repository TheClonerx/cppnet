#pragma once
#include <chrono>
#include <optional>
#include <system_error>
#include <utility>
#include <vector>

// if someone is going to use net::poll
// they may need some constats defined here
#include <poll.h>

namespace net {
class poll {
public:
    bool add(int fd, short events);
    bool modify(int fd, short events);
    bool remove(int fd);

    size_t execute(std::optional<std::chrono::milliseconds> timeout);
    size_t execute(std::optional<std::chrono::milliseconds> timeout, std::error_code&) noexcept;

#ifdef _GNU_SOURCE
    // for ppoll
    size_t execute(std::optional<std::chrono::nanoseconds> timeout, const sigset_t& sigmask);
    size_t execute(std::optional<std::chrono::nanoseconds> timeout, const sigset_t& sigmask, std::error_code&) noexcept;
#endif

    // ranges
    template <typename OIt>
    OIt get(OIt start, OIt stop) const noexcept(noexcept(*start = std::make_pair(0, 0)) && noexcept(++start == stop))
    {
        for (const pollfd& fd : fds) {
            if (start == stop)
                return stop;
            if (fd.revents) {
                *start = std::make_pair(fd.fd, fd.revents);
                ++start;
            }
        }
        return start;
    }

    // inserters
    template <typename OIt>
    OIt get(OIt it) const noexcept(noexcept(*it = std::make_pair(0, 0)) && noexcept(++it))
    {
        for (const pollfd& fd : fds) {
            if (fd.revents) {
                *it = std::make_pair(fd.fd, fd.revents);
                ++it;
            }
        }
        return it;
    }

private:
    std::vector<pollfd> fds;
};
} // net

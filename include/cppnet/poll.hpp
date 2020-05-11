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

#include <cppnet/socket.hpp>

namespace net {

class poll {
public:

    enum events {
        // basic events

        read = POLLRDNORM, // Normal data may be read without blocking
        write = POLLWRNORM, // Normal data may be written without blocking
        exception = POLLERR, // Always checked. An error occurred on the stream, see socket.error()

        // extra ones provided by poll

        hang_up = POLLHUP, // Disconnected
        invalid = POLLNVAL, // Invalid request

        high_priority_read = POLLRDBAND, // High priority data may be read without blocking
        high_priority_write = POLLWRBAND // High priority data may be read without blocking

        // TODO: Add the remaining flags
    };

    bool add(socket::native_handle_type fd, int events);
    bool modify(socket::native_handle_type fd, int events);
    bool remove(socket::native_handle_type fd);

    std::size_t execute(std::optional<std::chrono::milliseconds> timeout);
    std::size_t execute(std::optional<std::chrono::milliseconds> timeout, std::error_code&) noexcept;

#ifdef _GNU_SOURCE
    // for ppoll

    std::size_t execute(std::optional<std::chrono::nanoseconds> timeout, sigset_t const& sigmask);
    std::size_t execute(std::optional<std::chrono::nanoseconds> timeout, sigset_t const& sigmask, std::error_code&) noexcept;
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

} // namespace net

#pragma once
#ifndef __linux__
#error epoll is only avilable in linux
#endif

#include <chrono>
#include <optional>
#include <sys/epoll.h>
#include <system_error>
#include <vector>

#include <cppnet/socket.hpp>

namespace net {
class epoll {
public:
    // pollers must have a valid state when default constructed,
    // meaning, they should be able call execute in this state
    // so, the default constructor of epoll will create a valid poller using epoll_create(2)

    epoll();
    explicit epoll(int flags);
    epoll(int flags, std::error_code&) noexcept;

    using native_handle_type = int;

    epoll(const epoll&) = delete;
    epoll& operator=(const epoll&) = delete;

    epoll(epoll&&) noexcept;
    epoll& operator=(epoll&&) noexcept;

    ~epoll() noexcept;

    enum events {
        // basic ones

        read = EPOLLIN, // Data may be read without blocking
        write = EPOLLOUT, // Data may be written without blocking
        exception = EPOLLERR, // An error ocurred, see socket.error()

        // provided by epoll

        hang_up = EPOLLHUP,
        edge_triggered = EPOLLET, // Request for the events to be edge triggered, if not provided, epoll behaves similarly to poll, but faster

        // and more others that may be rarely used
        // TODO: Add the remaining flags
    };

    bool add(socket::native_handle_type fd, int events) noexcept;
    bool add(socket::native_handle_type fd, int events, std::error_code&) noexcept;

    bool modify(socket::native_handle_type fd, int events) noexcept;
    bool modify(socket::native_handle_type fd, int events, std::error_code&) noexcept;

    bool remove(socket::native_handle_type fd) noexcept;
    bool remove(socket::native_handle_type fd, std::error_code&) noexcept;

    std::size_t execute(std::optional<std::chrono::milliseconds> milliseconds);
    std::size_t execute(std::optional<std::chrono::milliseconds> milliseconds, std::error_code&);

    std::size_t execute(std::optional<std::chrono::milliseconds> milliseconds, const sigset_t& sigmask);
    std::size_t execute(std::optional<std::chrono::milliseconds> milliseconds, const sigset_t& sigmask, std::error_code&);

    native_handle_type native_handle() const noexcept;

    // ranges
    template <typename OIt>
    OIt get(OIt start, OIt stop) const noexcept(noexcept(*start = { 0, 0 }) && noexcept(++start == stop))
    {
        for (const epoll_event& item : data) {
            if (start == stop)
                return stop;
            *start = { item.data.fd, item.events };
            ++start;
        }
        return start;
    }

    // inserters
    template <typename OIt>
    OIt get(OIt it) const noexcept(noexcept(*it = { 0, 0 }) && noexcept(++it))
    {
        for (const epoll_event& item : data) {
            *it = { item.data.fd, item.events };
            ++it;
        }
        return it;
    }

    void close();
    void close(std::error_code&) noexcept;

protected:
    native_handle_type m_handle;
private:
    std::vector<epoll_event> data;
    size_t size = 0;
};
} // net

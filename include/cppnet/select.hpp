#pragma once
#include <chrono>
#include <optional>
#include <system_error>
#include <vector>

#include <cppnet/socket.hpp>

#ifndef CPPNET_IMPL
#include <cppnet/wsa_init.hpp>
#endif

namespace net {

struct select_return_t {
    std::size_t reads, writes, exceptions;
    constexpr operator std::size_t() const noexcept
    {
        return reads + writes + exceptions;
    }
};

class select {
public:
    enum events {
        read = 1 << 0, // Data may be read without blocking
        write = 1 << 1, // Data may be written without blocking
        exception = 1 << 2, // An error occurred, see socket.error()
    };

    bool add(socket::native_handle_type fd, int events);
    bool modify(socket::native_handle_type fd, int events);
    bool remove(socket::native_handle_type fd);

    select_return_t execute(std::optional<std::chrono::microseconds> timeout);
    select_return_t execute(std::optional<std::chrono::microseconds> timeout, std::error_code&) noexcept;

#ifdef _GNU_SOURCE
    select_return_t execute(std::optional<std::chrono::nanoseconds> timeout, const sigset_t& sigmask);
    select_return_t execute(std::optional<std::chrono::nanoseconds> timeout, const sigset_t& sigmask, std::error_code&) noexcept;
#endif

    // ranges

    template <typename OIt>
    OIt get(OIt start, OIt stop) const noexcept(noexcept(*start = { 0, 0 }) && noexcept(++start == stop))
    {
        for (const auto& item : fdlist) {
            if (start == stop)
                return stop;
            *start = { item.fd, item.sevents };
            ++start;
        }
        return start;
    }

    // inserters

    template <typename OIt>
    void get(OIt it) const noexcept(noexcept(*it = { 0, 0 }) && noexcept(++it))
    {
        for (const auto& item : fdlist) {
            *it = { item.fd, item.sevents };
            ++it;
        }
        return it;
    }

private:
    struct selectfd {
        socket::native_handle_type fd;
        int events; // events to select
        int sevents; // selected events
    };

    std::vector<selectfd> fdlist;
};

}

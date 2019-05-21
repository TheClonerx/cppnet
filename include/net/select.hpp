#pragma once
#include <chrono>
#include <list>
#include <optional>
#include <system_error>

namespace net {

struct select_return_t {
    size_t reads, writes, exceptions;
};

class select {
public:
    static constexpr int READ = 1;
    static constexpr int WRITE = 2;
    static constexpr int EXCEPT = 4;

    bool add(int fd, int events);
    bool modify(int fd, int events);
    bool remove(int fd);

    select_return_t execute(std::optional<std::chrono::microseconds> timeout);
    select_return_t execute(std::optional<std::chrono::microseconds> timeout, std::error_code&) noexcept;

#ifdef _GNU_SOURCE
    select_return_t execute(std::optional<std::chrono::nanoseconds> timeout, const sigset_t& sigmask);
    select_return_t execute(std::optional<std::chrono::nanoseconds> timeout, const sigset_t& sigmask, std::error_code&) noexcept;
#endif

    // ranges

    template <typename It>
    It get(It start, It stop, int events) const noexcept(noexcept(*start = 0) && noexcept(++start == stop))
    {
        for (const auto& item : fdlist) {
            if (start == stop)
                return stop;
            if (
                (events & READ   && item.sevents & READ  ) ||
                (events & WRITE  && item.sevents & WRITE ) ||
                (events & EXCEPT && item.sevents & EXCEPT)
            )
            {
                *start = item.fd;
                ++start;
            }
        }
        return start;
    }

    // inserters

    template <typename It>
    void get(It start, int events) const noexcept(noexcept(*start = 0) && noexcept(++start))
    {
        for (const auto& item : fdlist) {
            if (
                (events & READ   && item.sevents & READ  ) ||
                (events & WRITE  && item.sevents & WRITE ) ||
                (events & EXCEPT && item.sevents & EXCEPT)
            )
            {
                *start = item.fd;
                ++start;
            }
        }
        return start;
    }

private:
    struct selectfd {
        int fd;
        int events; // events to select
        int sevents; // selected events
    };

    std::list<selectfd> fdlist;
};
}

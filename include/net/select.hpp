#pragma once
#include <chrono>
#include <optional>
#include <system_error>
#include <vector>

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

    template <typename OIt>
    OIt get(OIt start, OIt stop) const noexcept(noexcept(*start = std::make_pair(0, 0)) && noexcept(++start == stop))
    {
        for (const auto& item : fdlist) {
            if (start == stop)
                return stop;
            *start = std::make_pair(item.fd, item.sevents);
            ++start;
        }
        return start;
    }

    // inserters

    template <typename OIt>
    void get(OIt it) const noexcept(noexcept(*it = std::make_pair(0, 0)) && noexcept(++it))
    {
        for (const auto& item : fdlist) {
            *it = std::make_pair(item.fd, item.sevents);
            ++it;
        }
        return it;
    }

private:
    struct selectfd {
        int fd;
        int events; // events to select
        int sevents; // selected events
    };

    std::vector<selectfd> fdlist;
};
}

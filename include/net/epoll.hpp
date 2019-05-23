#pragma once
#ifndef __linux__
#error epoll is only avilable for linux
#endif

#include <chrono>
#include <optional>
#include <sys/epoll.h>
#include <system_error>
#include <vector>

namespace net {
class epoll {
public:
    epoll();
    explicit epoll(int flags);
    epoll(int flags, std::error_code&) noexcept;

    epoll(const epoll&) = delete;
    epoll& operator=(const epoll&) = delete;

    epoll(epoll&&) noexcept;
    epoll& operator=(epoll&&) noexcept;

    ~epoll() noexcept;

    bool add(int fd, int events) noexcept;
    bool add(int fd, int events, std::error_code&) noexcept;

    bool modify(int fd, int events) noexcept;
    bool modify(int fd, int events, std::error_code&) noexcept;

    bool remove(int fd) noexcept;
    bool remove(int fd, std::error_code&) noexcept;

    size_t execute(std::optional<std::chrono::milliseconds> milliseconds);
    size_t execute(std::optional<std::chrono::milliseconds> milliseconds, std::error_code&);

    size_t execute(std::optional<std::chrono::milliseconds> milliseconds, const sigset_t& sigmask);
    size_t execute(std::optional<std::chrono::milliseconds> milliseconds, const sigset_t& sigmask, std::error_code&);

    int fileno() const noexcept;

    // ranges
    template <typename OIt>
    OIt get(OIt start, OIt stop) const noexcept(noexcept(*start = std::make_pair(0, 0)) && noexcept(++start == stop))
    {
        for (const epoll_event& item : data) {
            if (start == stop)
                return stop;
            *start = std::make_pair(item.data.fd, item.events);
            ++start;
        }
        return start;
    }

    // inserters
    template <typename OIt>
    OIt get(OIt it) const noexcept(noexcept(*it = std::make_pair(0, 0)) && noexcept(++it))
    {
        for (const epoll_event& item : data) {
            *it = std::make_pair(item.data.fd, item.events);
            ++it;
        }
        return it;
    }

    void close();
    void close(std::error_code&) noexcept;

private:
    int fd;
    std::vector<epoll_event> data;
    size_t size;
};
} // net

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
    template <typename It>
    It get(It start, It stop) const noexcept(noexcept(std::get<0>(*start) = 0) && noexcept(std::get<1>(*start) = 0) && noexcept(++start == stop))
    {
        for (const epoll_event& item : data) {
            if (start == stop)
                return stop;
            std::get<0>(*start) = item.data.fd;
            std::get<1>(*start) = item.events;
            ++start;
        }
        return start;
    }

    // inserters
    template <typename It>
    It get(It it) const noexcept(noexcept(std::get<0>(*it) = 0) && noexcept(std::get<1>(*it) = 0) && noexcept(++it))
    {
        for (const epoll_event& item : data) {
            std::get<0>(*it) = item.data.fd;
            std::get<1>(*it) = item.events;
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

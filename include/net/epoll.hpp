#pragma once
#ifndef __linux__
#error epoll is only avilable for linux
#endif

#include <system_error>
#include <chrono>
#include <vector>
#include <optional>
#include <sys/epoll.h>

namespace net
{
class epoll
{
public:
    epoll();
    explicit epoll(int);
    epoll(int, std::error_code&) noexcept;

    epoll(const epoll&) = delete;
    epoll& operator=(const epoll&) = delete;

    epoll(epoll&&) noexcept;
    epoll& operator=(epoll&&) noexcept;

    bool add(int, int) noexcept;
    bool add(int, int, std::error_code&) noexcept;

    bool modify(int, int) noexcept;
    bool modify(int, int, std::error_code&) noexcept;

    bool remove(int) noexcept;
    bool remove(int, std::error_code&) noexcept;

    size_t execute(std::optional<std::chrono::milliseconds>);
    size_t execute(std::optional<std::chrono::milliseconds>, std::error_code&);

    size_t execute(std::optional<std::chrono::milliseconds>, const sigset_t&);
    size_t execute(std::optional<std::chrono::milliseconds>, const sigset_t&, std::error_code&);

    int fileno() const noexcept;

    // ranges
    template <typename It>
    It get(It start, It stop) const noexcept(noexcept(std::get<0>(*start) = 0) && noexcept(std::get<1>(*start) = 0) && noexcept(++start == stop))
    {
        for (const epoll_event& item : data)
        {
            if (start == stop) return stop;
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
        for (const epoll_event& item : data)
        {
            std::get<0>(*it) = item.data.fd;
            std::get<1>(*it) = item.events;
            ++it;
        }
        return it;
    }
private:
    int fd;
    std::vector<epoll_event> data;
    size_t size;
};
} // net

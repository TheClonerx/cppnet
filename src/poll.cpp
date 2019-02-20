#include "net/poll.hpp"
#include <algorithm>
#include <poll.h>

bool net::poll::add(int fd, short eventmask)
{
    if (fd < 0)
        return false;
    auto it = std::find_if(fds.begin(), fds.end(), [fd](const pollfd& p) {
        return p.fd == fd;
    });

    if (it != fds.end())
        return false;
    fds.push_back(pollfd { fd, eventmask, 0 });

    return true;
}

bool net::poll::modify(int fd, short eventmask)
{
    if (fd < 0)
        return false;
    auto it = std::find_if(fds.begin(), fds.end(), [fd](const pollfd& p) {
        return p.fd == fd;
    });

    if (it == fds.end())
        return false;

    it->events = eventmask;
    return true;
}

bool net::poll::unregister(int fd)
{
    if (fd < 0)
        return false;
    auto it = std::find_if(fds.begin(), fds.end(), [fd](const pollfd& p) {
        return p.fd == fd;
    });

    if (it == fds.end())
        return false;

    fds.erase(it);
    return true;
}

size_t net::poll::execute(std::chrono::milliseconds timeout)
{
    for (pollfd& fd : fds)
        fd.revents = 0;
    int ret = ::poll(fds.data(), fds.size(), timeout.count());
    if (ret < 0)
        throw std::system_error(errno, std::system_category());
    return ret;
}

size_t net::poll::execute(std::chrono::milliseconds timeout, std::error_code& e) noexcept
{
    for (pollfd& fd : fds)
        fd.revents = 0;
    int ret = ::poll(
        fds.data(),
        fds.size(),
        std::chrono::duration_cast<std::chrono::duration<int, std::milli>>(timeout).count());
    if (ret < 0)
        e.assign(errno, std::system_category());
    return ret;
}

#ifdef _GNU_SOURCE
size_t net::poll::execute(std::chrono::nanoseconds timeout, const sigset_t& sigmask)
{
    timespec tm;
    tm.tv_sec = std::chrono::duration_cast<std::chrono::duration<long>>(timeout).count();
    tm.tv_nsec = timeout.count() % std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::seconds(1)).count();
    for (pollfd& fd : fds)
        fd.revents = 0;

    int ret = ::ppoll(fds.data(), fds.size(), &tm, &sigmask);
    if (ret < 0)
        throw std::system_error(errno, std::system_category());
    return ret;
}

size_t net::poll::execute(std::chrono::nanoseconds timeout, const sigset_t& sigmask, std::error_code& e) noexcept
{
    timespec tm;
    tm.tv_sec = std::chrono::duration_cast<std::chrono::duration<long>>(timeout).count();
    tm.tv_nsec = timeout.count() % std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::seconds(1)).count();
    
    for (pollfd& fd : fds)
        fd.revents = 0;

    int ret = ::ppoll(fds.data(), fds.size(), &tm, &sigmask);
    if (ret < 0)
        e.assign(errno, std::system_category());
    return ret;
}
#endif
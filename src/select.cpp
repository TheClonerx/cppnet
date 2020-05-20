#include <algorithm>
#include <cppnet/select.hpp>

bool net::select::add(socket::native_handle_type fd, int events)
{

    auto it = std::find_if(fdlist.begin(), fdlist.end(), [fd](const selectfd& item) {
        return item.fd == fd;
    });

    if (it != fdlist.end())
        return false;

    fdlist.emplace_back(selectfd { fd, events, 0 });
    return true;
}

bool net::select::remove(socket::native_handle_type fd)
{
    auto it = std::find_if(fdlist.begin(), fdlist.end(), [fd](const selectfd& item) {
        return item.fd == fd;
    });

    if (it == fdlist.end())
        return false;

    fdlist.erase(it);

    return true;
}

net::select_return_t net::select::execute(std::optional<std::chrono::microseconds> timeout)
{
    std::error_code e;
    auto result = execute(timeout, e);
    if (e)
        throw std::system_error(e);
    return result;
}

net::select_return_t net::select::execute(std::optional<std::chrono::microseconds> timeout, std::error_code& e) noexcept
{
    using namespace std::chrono;
    using namespace std::literals;
    if (fdlist.empty())
        return { 0, 0, 0 };

    fd_set rlist;
    fd_set wlist;
    fd_set xlist;

    FD_ZERO(&rlist);
    FD_ZERO(&wlist);
    FD_ZERO(&xlist);

    int maxfd = 0;

    for (auto& item : fdlist) {
        item.sevents = 0;
#ifndef _WIN32
        if (maxfd < item.fd)
            maxfd = item.fd;
#endif
        if (item.events & events::read)
            FD_SET(item.fd, &rlist);
        if (item.events & events::write)
            FD_SET(item.fd, &wlist);
        if (item.events & events::exception)
            FD_SET(item.fd, &xlist);
    }
    maxfd++;

    {
        timeval tm;
        if (timeout) {
            tm.tv_sec = static_cast<decltype(tm.tv_sec)>((*timeout).count());
            tm.tv_usec = static_cast<decltype(tm.tv_sec)>((*timeout % 1s).count());
        }
        int ret = ::select(maxfd, &rlist, &wlist, &xlist, timeout ? &tm : nullptr);
        if (ret < 0) {
            e.assign(errno, std::system_category());
            return { 0, 0, 0 };
        }
    }

    net::select_return_t ret { 0, 0, 0 };

    for (auto& item : fdlist) {
        if (FD_ISSET(item.fd, &rlist)) {
            item.sevents = item.sevents | events::read;
            ret.reads++;
        } else if (FD_ISSET(item.fd, &wlist)) {
            item.sevents = item.sevents | events::write;
            ret.writes++;
        } else if (FD_ISSET(item.fd, &xlist)) {
            item.sevents = item.sevents | events::exception;
            ret.exceptions++;
        }
    }

    return ret;
}

#ifdef _GNU_SOURCE

net::select_return_t net::select::execute(std::optional<std::chrono::nanoseconds> timeout, const sigset_t& sigmask)
{
    std::error_code e;
    auto result = execute(timeout, sigmask, e);
    if (e)
        throw std::system_error(e);
    return result;
}

net::select_return_t net::select::execute(std::optional<std::chrono::nanoseconds> timeout, const sigset_t& sigmask, std::error_code& e) noexcept
{
    if (fdlist.empty())
        return { 0, 0, 0 };

    fd_set rlist;
    fd_set wlist;
    fd_set xlist;

    FD_ZERO(&rlist);
    FD_ZERO(&wlist);
    FD_ZERO(&xlist);

    int maxfd = 0;
    for (auto& item : fdlist) {
        item.sevents = 0;
        if (maxfd < item.fd)
            maxfd = item.fd;

        if (item.events & events::read)
            FD_SET(item.fd, &rlist);
        if (item.events & events::write)
            FD_SET(item.fd, &wlist);
        if (item.events & events::exception)
            FD_SET(item.fd, &xlist);
    }
    maxfd++;

    {
        using namespace std::chrono;
        using namespace std::literals;

        timespec tm;
        if (timeout) {
            tm.tv_sec = duration_cast<seconds>(*timeout).count();
            tm.tv_nsec = (*timeout % 1s).count();
        }
        int ret = ::pselect(maxfd, &rlist, &wlist, &xlist, timeout ? &tm : nullptr, &sigmask);
        if (ret < 0) {
            e.assign(errno, std::system_category());
            return { 0, 0, 0 };
        }
    }

    net::select_return_t ret { 0, 0, 0 };

    for (auto& item : fdlist) {
        if (FD_ISSET(item.fd, &rlist)) {
            item.sevents = item.sevents | events::read;
            ret.reads++;
        } else if (FD_ISSET(item.fd, &wlist)) {
            item.sevents = item.sevents | events::write;
            ret.reads++;
        } else if (FD_ISSET(item.fd, &xlist)) {
            item.sevents = item.sevents | events::exception;
            ret.exceptions++;
        }
    }

    return ret;
}

#endif

#include "net/select.hpp"
#include <algorithm>

bool net::select::add(int fd, net::select::EventFlags events)
{
    auto it = std::find_if(fdlist.begin(), fdlist.end(), [fd](const Item& item) {
        return item.fd == fd;
    });

    if (it != fdlist.end()) {
        EventFlags prev = it->events;
        it->events = it->events | events;
        if (prev == it->events)
            return false;
        return true;
    }

    fdlist.emplace_back(Item { fd, events, static_cast<net::select::EventFlags>(0) });
    return true;
}

bool net::select::remove(int fd, net::select::EventFlags events)
{
    auto it = std::find_if(fdlist.begin(), fdlist.end(), [fd](const Item& item) {
        return item.fd == fd;
    });

    if (it == fdlist.end())
        return false;

    net::select::EventFlags prev = it->events;
    it->events = it->events ^ events;

    if (!it->events) {
        fdlist.erase(it);
        return true;
    }

    if (prev != it->events)
        return true;
    return false;
}

net::select_return_t net::select::execute(std::optional<std::chrono::microseconds> timeout)
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
        item.sevents = static_cast<net::select::EventFlags>(0);
        if (maxfd < item.fd)
            maxfd = item.fd;

        if (static_cast<bool>(item.events & net::select::EventFlags::read))
            FD_SET(item.fd, &rlist);
        if (static_cast<bool>(item.events & net::select::EventFlags::write))
            FD_SET(item.fd, &wlist);
        if (static_cast<bool>(item.events & net::select::EventFlags::except))
            FD_SET(item.fd, &xlist);
    }
    maxfd++;

    {
        timeval tm;
        if (timeout) {
            tm.tv_sec = std::chrono::duration_cast<std::chrono::seconds>(*timeout).count();
            tm.tv_usec = timeout->count() % std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::seconds(1)).count();
        }
        int ret = ::select(maxfd, &rlist, &wlist, &xlist, timeout ? &tm : nullptr);
        if (ret < 0)
            throw std::system_error(errno, std::system_category());
    }

    net::select_return_t ret { 0, 0, 0 };

    for (Item& item : fdlist) {
        if (FD_ISSET(item.fd, &rlist)) {
            item.sevents = item.sevents | net::select::EventFlags::read;
            ret.reads++;
        } else if (FD_ISSET(item.fd, &wlist)) {
            item.sevents = item.sevents | net::select::EventFlags::write;
            ret.reads++;
        } else if (FD_ISSET(item.fd, &xlist)) {
            item.sevents = item.sevents | net::select::EventFlags::except;
            ret.exceptions++;
        }
    }

    return ret;
}

net::select_return_t net::select::execute(std::optional<std::chrono::microseconds> timeout, std::error_code& e) noexcept
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
        item.sevents = static_cast<net::select::EventFlags>(0);
        if (maxfd < item.fd)
            maxfd = item.fd;

        if (static_cast<bool>(item.events & net::select::EventFlags::read))
            FD_SET(item.fd, &rlist);
        if (static_cast<bool>(item.events & net::select::EventFlags::write))
            FD_SET(item.fd, &wlist);
        if (static_cast<bool>(item.events & net::select::EventFlags::except))
            FD_SET(item.fd, &xlist);
    }
    maxfd++;

    {
        timeval tm;
        if (timeout) {
            tm.tv_sec = std::chrono::duration_cast<std::chrono::seconds>(*timeout).count();
            tm.tv_usec = timeout->count() % std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::seconds(1)).count();
        }
        int ret = ::select(maxfd, &rlist, &wlist, &xlist, timeout ? &tm : nullptr);
        if (ret < 0) {
            e.assign(errno, std::system_category());
            return { 0, 0, 0 };
        }
    }

    net::select_return_t ret { 0, 0, 0 };

    for (Item& item : fdlist) {
        if (FD_ISSET(item.fd, &rlist)) {
            item.sevents = item.sevents | net::select::EventFlags::read;
            ret.reads++;
        } else if (FD_ISSET(item.fd, &wlist)) {
            item.sevents = item.sevents | net::select::EventFlags::write;
            ret.writes++;
        } else if (FD_ISSET(item.fd, &xlist)) {
            item.sevents = item.sevents | net::select::EventFlags::except;
            ret.exceptions++;
        }
    }

    return ret;
}

#ifdef _GNU_SOURCE

net::select_return_t net::select::execute(std::optional<std::chrono::nanoseconds> timeout, const sigset_t& sigmask)
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
        item.sevents = static_cast<net::select::EventFlags>(0);
        if (maxfd < item.fd)
            maxfd = item.fd;

        if (static_cast<bool>(item.events & net::select::EventFlags::read))
            FD_SET(item.fd, &rlist);
        if (static_cast<bool>(item.events & net::select::EventFlags::write))
            FD_SET(item.fd, &wlist);
        if (static_cast<bool>(item.events & net::select::EventFlags::except))
            FD_SET(item.fd, &xlist);
    }
    maxfd++;

    {
        timespec tm;
        if (timeout) {
            tm.tv_sec = std::chrono::duration_cast<std::chrono::seconds>(*timeout).count();
            tm.tv_nsec = timeout->count() % std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::seconds(1)).count();
        }
        int ret = ::pselect(maxfd, &rlist, &wlist, &xlist, timeout ? &tm : nullptr, &sigmask);
        if (ret < 0)
            throw std::system_error(errno, std::system_category());
    }

    net::select_return_t ret { 0, 0, 0 };

    for (Item& item : fdlist) {
        if (FD_ISSET(item.fd, &rlist)) {
            item.sevents = item.sevents | net::select::EventFlags::read;
            ret.reads++;
        } else if (FD_ISSET(item.fd, &wlist)) {
            item.sevents = item.sevents | net::select::EventFlags::write;
            ret.reads++;
        } else if (FD_ISSET(item.fd, &xlist)) {
            item.sevents = item.sevents | net::select::EventFlags::except;
            ret.exceptions++;
        }
    }

    return ret;
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
        item.sevents = static_cast<net::select::EventFlags>(0);
        if (maxfd < item.fd)
            maxfd = item.fd;

        if (static_cast<bool>(item.events & net::select::EventFlags::read))
            FD_SET(item.fd, &rlist);
        if (static_cast<bool>(item.events & net::select::EventFlags::write))
            FD_SET(item.fd, &wlist);
        if (static_cast<bool>(item.events & net::select::EventFlags::except))
            FD_SET(item.fd, &xlist);
    }
    maxfd++;

    {
        timespec tm;
        if (timeout) {
            tm.tv_sec = std::chrono::duration_cast<std::chrono::seconds>(*timeout).count();
            tm.tv_nsec = timeout->count() % std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::seconds(1)).count();
        }
        int ret = ::pselect(maxfd, &rlist, &wlist, &xlist, timeout ? &tm : nullptr, &sigmask);
        if (ret < 0) {
            e.assign(errno, std::system_category());
            return { 0, 0, 0 };
        }
    }

    net::select_return_t ret { 0, 0, 0 };

    for (Item& item : fdlist) {
        if (FD_ISSET(item.fd, &rlist)) {
            item.sevents = item.sevents | net::select::EventFlags::read;
            ret.reads++;
        } else if (FD_ISSET(item.fd, &wlist)) {
            item.sevents = item.sevents | net::select::EventFlags::write;
            ret.reads++;
        } else if (FD_ISSET(item.fd, &xlist)) {
            item.sevents = item.sevents | net::select::EventFlags::except;
            ret.exceptions++;
        }
    }

    return ret;
}

#endif

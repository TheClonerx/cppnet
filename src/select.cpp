#include "net/select.hpp"
#include <algorithm>

bool net::select::add_read(int fd)
{
    auto it = std::find_if(fdlist.begin(), fdlist.end(), [fd](const auto& t) {
        return std::get<0>(t) == fd && std::get<1>(t) == Type::READ;
    });

    if (it != fdlist.end())
        return false;

    fdlist.emplace_back(fd, Type::READ, false);
    return true;
}

bool net::select::add_write(int fd)
{
    auto it = std::find_if(fdlist.begin(), fdlist.end(), [fd](const auto& t) {
        return std::get<0>(t) == fd && std::get<1>(t) == Type::WRITE;
    });

    if (it != fdlist.end())
        return false;

    fdlist.emplace_back(fd, Type::WRITE, false);
    return true;
}

bool net::select::add_except(int fd)
{
    auto it = std::find_if(fdlist.begin(), fdlist.end(), [fd](const auto& t) {
        return std::get<0>(t) == fd && std::get<1>(t) == Type::EXCEPT;
    });

    if (it != fdlist.end())
        return false;

    fdlist.emplace_back(fd, Type::EXCEPT, false);
    return true;
}

bool net::select::remove_read(int fd)
{
    auto it = std::find_if(fdlist.begin(), fdlist.end(), [fd](const auto& t) {
        return std::get<0>(t) == fd && std::get<1>(t) == Type::READ;
    });

    if (it == fdlist.end())
        return false;

    fdlist.erase(it);
    return true;
}

bool net::select::remove_write(int fd)
{
    auto it = std::find_if(fdlist.begin(), fdlist.end(), [fd](const auto& t) {
        return std::get<0>(t) == fd && std::get<1>(t) == Type::WRITE;
    });

    if (it == fdlist.end())
        return false;

    fdlist.erase(it);
    return true;
}

bool net::select::remove_except(int fd)
{
    auto it = std::find_if(fdlist.begin(), fdlist.end(), [fd](const auto& t) {
        return std::get<0>(t) == fd && std::get<1>(t) == Type::EXCEPT;
    });

    if (it == fdlist.end())
        return false;

    fdlist.erase(it);
    return true;
}

std::tuple<size_t, size_t, size_t> net::select::execute(std::optional<std::chrono::microseconds> timeout)
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

    for (auto& t : fdlist) {
        std::get<2>(t) = false;
        if (maxfd < std::get<0>(t))
            maxfd = std::get<0>(t);

        switch (std::get<1>(t)) {
        case Type::READ:
            FD_SET(std::get<0>(t), &rlist);
            break;

        case Type::WRITE:
            FD_SET(std::get<0>(t), &wlist);
            break;
        case Type::EXCEPT:
            FD_SET(std::get<0>(t), &xlist);
            break;
        }
    }
    maxfd++;

    timeval tm;
    if (timeout) {
        tm.tv_sec = std::chrono::duration_cast<std::chrono::seconds>(*timeout).count();
        tm.tv_usec = timeout->count() % std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::seconds(1)).count();
    }

    {
        int ret = ::select(maxfd, &rlist, &wlist, &xlist, timeout ? &tm : nullptr);
        if (ret < 0)
            throw std::system_error(errno, std::system_category());
    }

    std::tuple<size_t, size_t, size_t> ret { 0, 0, 0 };

    for (const auto& t : fdlist) {
        if (FD_ISSET(std::get<0>(t), &rlist))
            std::get<0>(ret)++;
        else if (FD_ISSET(std::get<0>(t), &wlist))
            std::get<1>(ret)++;
        else if (FD_ISSET(std::get<0>(t), &xlist))
            std::get<2>(ret)++;
    }

    return ret;
}

std::tuple<size_t, size_t, size_t> net::select::execute(std::optional<std::chrono::microseconds> timeout, std::error_code& e) noexcept
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

    for (auto& t : fdlist) {
        std::get<2>(t) = false;
        if (maxfd < std::get<0>(t))
            maxfd = std::get<0>(t);

        switch (std::get<1>(t)) {
        case Type::READ:
            FD_SET(std::get<0>(t), &rlist);
            break;

        case Type::WRITE:
            FD_SET(std::get<0>(t), &wlist);
            break;
        case Type::EXCEPT:
            FD_SET(std::get<0>(t), &xlist);
            break;
        }
    }
    maxfd++;

    timeval tm;
    if (timeout) {
        tm.tv_sec = std::chrono::duration_cast<std::chrono::seconds>(*timeout).count();
        tm.tv_usec = timeout->count() % std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::seconds(1)).count();
    }

    {
        int ret = ::select(maxfd, &rlist, &wlist, &xlist, timeout ? &tm : nullptr);
        if (ret < 0) {
            e.assign(errno, std::system_category());
            return { 0, 0, 0 };
        }
    }

    std::tuple<size_t, size_t, size_t> ret { 0, 0, 0 };

    for (const auto& t : fdlist) {
        if (FD_ISSET(std::get<0>(t), &rlist))
            std::get<0>(ret)++;
        else if (FD_ISSET(std::get<0>(t), &wlist))
            std::get<1>(ret)++;
        else if (FD_ISSET(std::get<0>(t), &xlist))
            std::get<2>(ret)++;
    }

    return ret;
}

#ifdef _GNU_SOURCE
std::tuple<size_t, size_t, size_t> net::select::execute(std::optional<std::chrono::nanoseconds> timeout, const sigset_t& sigmask)
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

    for (auto& t : fdlist) {
        std::get<2>(t) = false;
        if (maxfd < std::get<0>(t))
            maxfd = std::get<0>(t);

        switch (std::get<1>(t)) {
        case Type::READ:
            FD_SET(std::get<0>(t), &rlist);
            break;

        case Type::WRITE:
            FD_SET(std::get<0>(t), &wlist);
            break;
        case Type::EXCEPT:
            FD_SET(std::get<0>(t), &xlist);
            break;
        }
    }
    maxfd++;

    timespec tm;
    if (timeout) {
        tm.tv_sec = std::chrono::duration_cast<std::chrono::seconds>(*timeout).count();
        tm.tv_nsec = timeout->count() % std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::seconds(1)).count();
    }

    {
        int ret = ::pselect(maxfd, &rlist, &wlist, &xlist, timeout ? &tm : nullptr, &sigmask);
        if (ret < 0)
            throw std::system_error(errno, std::system_category());
    }

    std::tuple<size_t, size_t, size_t> ret { 0, 0, 0 };

    for (const auto& t : fdlist) {
        if (FD_ISSET(std::get<0>(t), &rlist))
            std::get<0>(ret)++;
        else if (FD_ISSET(std::get<0>(t), &wlist))
            std::get<1>(ret)++;
        else if (FD_ISSET(std::get<0>(t), &xlist))
            std::get<2>(ret)++;
    }

    return ret;
}

std::tuple<size_t, size_t, size_t> net::select::execute(std::optional<std::chrono::nanoseconds> timeout, const sigset_t& sigmask, std::error_code& e) noexcept
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

    for (auto& t : fdlist) {
        std::get<2>(t) = false;
        if (maxfd < std::get<0>(t))
            maxfd = std::get<0>(t);

        switch (std::get<1>(t)) {
        case Type::READ:
            FD_SET(std::get<0>(t), &rlist);
            break;

        case Type::WRITE:
            FD_SET(std::get<0>(t), &wlist);
            break;
        case Type::EXCEPT:
            FD_SET(std::get<0>(t), &xlist);
            break;
        }
    }
    maxfd++;

    timespec tm;
    if (timeout) {
        tm.tv_sec = std::chrono::duration_cast<std::chrono::seconds>(*timeout).count();
        tm.tv_nsec = timeout->count() % std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::seconds(1)).count();
    }

    {
        int ret = ::pselect(maxfd, &rlist, &wlist, &xlist, timeout ? &tm : nullptr, &sigmask);
        if (ret < 0) {
            e.assign(errno, std::system_category());
            return { 0, 0, 0 };
        }
    }

    std::tuple<size_t, size_t, size_t> ret { 0, 0, 0 };

    for (const auto& t : fdlist) {
        if (FD_ISSET(std::get<0>(t), &rlist))
            std::get<0>(ret)++;
        else if (FD_ISSET(std::get<0>(t), &wlist))
            std::get<1>(ret)++;
        else if (FD_ISSET(std::get<0>(t), &xlist))
            std::get<2>(ret)++;
    }

    return ret;
}

#endif
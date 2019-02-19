#include "poll.hpp"
#include <poll.h>

void net::poll::add(int fd, int eventmask)
{
    // If the value of fd is less than 0, events shall be ignored...
    if (fd >= 0)
        fds.try_emplace(fd, eventmask);
}

void net::poll::modify(int fd, int eventmask)
{
    fds.insert_or_assign(fd, eventmask);
}

void net::poll::unregister(int fd)
{
    fds.erase(fd);
}

std::unordered_map<int, int> net::poll::get(std::chrono::duration<int, std::milli> timeout)
{
    std::vector<pollfd> pollfds;
    pollfds.reserve(fds.size());
    for (const auto& p : fds)
        pollfds.emplace_back(p.first, p.second, 0);
    {
        int ret = ::poll(pollfds.data(), pollfds.size(), timeout.count());
        if (ret < 0)
            throw std::system_error(errno, std::system_category());
        else if (!ret)
            return {};
    }

    std::unordered_map<int, int> ret;
    for (const auto& fd : pollfds)
        if (fd.revents)
            ret.try_emplace(fd.fd, fd.revents);
    return ret;
}

std::unordered_map<int, int> net::poll::get(std::chrono::duration<int, std::milli> timeout, std::error_code& e)
{
    std::vector<pollfd> pollfds;
    pollfds.reserve(fds.size());
    for (const auto& p : fds)
        pollfds.emplace_back(p.first, p.second, 0);
    {
        int ret = ::poll(pollfds.data(), pollfds.size(), timeout.count());
        if (ret < 0) {
            e.assign(errno, std::system_category());
            return {};
            e.assign(0, std::system_category());
            if (!ret)
                return {};
        }
    }
    std::unordered_map<int, int> ret;
    for (const auto& fd : pollfds)
        if (fd.revents)
            ret.try_emplace(fd.fd, fd.revents);
    return ret;
}
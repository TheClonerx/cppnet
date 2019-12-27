#ifdef __linux__
#include <cppnet/epoll.hpp>
#include <unistd.h>

net::epoll::epoll()
    : epoll(0)
{
    if (fd < 0)
        throw std::system_error(errno, std::system_category());
}

net::epoll::epoll(int flags)
    : fd(epoll_create1(flags))
{
    if (fd < 0)
        throw std::system_error(errno, std::system_category());
}

net::epoll::epoll(int flags, std::error_code& e) noexcept
    : fd(epoll_create1(flags))
{
    if (fd < 0)
        e.assign(errno, std::system_category());
    else
        e.assign(0, std::system_category());
}

net::epoll::epoll(net::epoll&& rhs) noexcept
    : fd(rhs.fd)
{
    rhs.fd = -1;
}

net::epoll::~epoll() noexcept
{
    ::close(fd);
}

net::epoll& net::epoll::operator=(net::epoll&& rhs) noexcept
{
    fd = rhs.fd;
    rhs.fd = -1;
    return *this;
}

bool net::epoll::add(int fd, int events) noexcept
{
    epoll_event event;
    event.events = events;
    event.data.fd = fd;

    int ret = epoll_ctl(this->fd, EPOLL_CTL_ADD, fd, &event);
    if (ret < 0)
        return false;
    else {
        ++size;
        return true;
    }
}

bool net::epoll::add(int fd, int events, std::error_code& e) noexcept
{
    epoll_event event;
    event.events = events;
    event.data.fd = fd;

    int ret = epoll_ctl(this->fd, EPOLL_CTL_ADD, fd, &event);

    if (ret < 0) {
        e.assign(errno, std::system_category());
        return false;
    } else {
        e.assign(0, std::system_category());
        ++size;
        return true;
    }
}

bool net::epoll::modify(int fd, int events) noexcept
{
    epoll_event event;
    event.events = events;
    event.data.fd = fd;

    return !epoll_ctl(this->fd, EPOLL_CTL_MOD, fd, &event);
}

bool net::epoll::modify(int fd, int events, std::error_code& e) noexcept
{
    epoll_event event;
    event.events = events;
    event.data.fd = fd;

    int ret = epoll_ctl(this->fd, EPOLL_CTL_MOD, fd, &event);

    if (ret < 0) {
        e.assign(errno, std::system_category());
        return false;
    } else {
        e.assign(0, std::system_category());
        return true;
    }
}

bool net::epoll::remove(int fd) noexcept
{
    int ret = epoll_ctl(this->fd, EPOLL_CTL_DEL, fd, nullptr);
    if (ret < 0)
        return false;
    else {
        --size;
        return true;
    }
}

bool net::epoll::remove(int fd, std::error_code& e) noexcept
{
    int ret = epoll_ctl(this->fd, EPOLL_CTL_DEL, fd, nullptr);

    if (ret < 0) {
        e.assign(errno, std::system_category());
        return false;
    } else {
        e.assign(0, std::system_category());
        --size;
        return true;
    }
}

size_t net::epoll::execute(std::optional<std::chrono::milliseconds> timeout)
{
    data.clear();
    data.insert(data.begin(), size, epoll_event {});
    int ret = epoll_wait(fd, data.data(), size, timeout ? timeout->count() : -1);
    if (ret < 0)
        throw std::system_error(errno, std::system_category());
    return ret;
}

size_t net::epoll::execute(std::optional<std::chrono::milliseconds> timeout, std::error_code& e)
{
    data.clear();
    data.insert(data.begin(), size, epoll_event {}); // this may throw sadly
    int ret = epoll_wait(fd, data.data(), size, timeout ? timeout->count() : -1);
    if (ret < 0) {
        e.assign(errno, std::system_category());
        return 0;
    } else {
        e.assign(0, std::system_category());
        return ret;
    }
}

size_t net::epoll::execute(std::optional<std::chrono::milliseconds> timeout, const sigset_t& sigmask)
{
    data.clear();
    data.insert(data.begin(), size, epoll_event {});
    int ret = epoll_pwait(fd, data.data(), size, timeout ? timeout->count() : -1, &sigmask);
    if (ret < 0)
        throw std::system_error(errno, std::system_category());
    return ret;
}

size_t net::epoll::execute(std::optional<std::chrono::milliseconds> timeout, const sigset_t& sigmask, std::error_code& e)
{
    data.clear();
    data.insert(data.begin(), size, epoll_event {}); // this may throw sadly
    int ret = epoll_pwait(fd, data.data(), size, timeout ? timeout->count() : -1, &sigmask);
    if (ret < 0) {
        e.assign(errno, std::system_category());
        return 0;
    } else {
        e.assign(0, std::system_category());
        return ret;
    }
}

void net::epoll::close()
{
    if (::close(fd) < 0)
        throw std::system_error(errno, std::system_category());
    fd = -1;
}

void net::epoll::close(std::error_code& e) noexcept
{
    if (::close(fd) < 0)
        e.assign(errno, std::system_category());
    else {
        e.assign(0, std::system_category());
        fd = -1;
    }
}

#endif

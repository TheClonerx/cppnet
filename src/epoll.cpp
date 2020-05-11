#ifdef __linux__
#include <cppnet/epoll.hpp>
#include <unistd.h>

net::epoll::epoll()
    : epoll(0) // can throw
{
}

net::epoll::epoll(int flags)
    : m_handle(epoll_create1(flags))
{
    if (m_handle < 0)
        throw std::system_error(errno, std::system_category());
}

net::epoll::epoll(int flags, std::error_code& e) noexcept
    : m_handle(epoll_create1(flags))
{
    if (m_handle < 0)
        e.assign(errno, std::system_category());
    else
        e.assign(0, std::system_category());
}

net::epoll::epoll(net::epoll&& rhs) noexcept
    : m_handle(rhs.m_handle)
{
    rhs.m_handle = -1;
}

net::epoll::~epoll() noexcept
{
    if (m_handle != -1)
        ::close(m_handle);
    m_handle = -1;
}

net::epoll& net::epoll::operator=(net::epoll&& rhs) noexcept
{
    m_handle = std::exchange(rhs.m_handle, -1);
    return *this;
}

bool net::epoll::add(socket::native_handle_type fd, int events) noexcept
{
    epoll_event event{};
    event.events = events;
    event.data.fd = fd;

    int ret = epoll_ctl(m_handle, EPOLL_CTL_ADD, fd, &event);
    if (ret < 0)
        return false;
    else {
        ++size;
        return true;
    }
}

bool net::epoll::add(socket::native_handle_type fd, int events, std::error_code& e) noexcept
{
    epoll_event event;
    event.events = events;
    event.data.fd = fd;

    int ret = epoll_ctl(m_handle, EPOLL_CTL_ADD, fd, &event);

    if (ret < 0) {
        e.assign(errno, std::system_category());
        return false;
    } else {
        e.assign(0, std::system_category());
        ++size;
        return true;
    }
}

bool net::epoll::modify(socket::native_handle_type fd, int events) noexcept
{
    epoll_event event;
    event.events = events;
    event.data.fd = fd;

    return !epoll_ctl(m_handle, EPOLL_CTL_MOD, fd, &event);
}

bool net::epoll::modify(socket::native_handle_type fd, int events, std::error_code& e) noexcept
{
    epoll_event event;
    event.events = events;
    event.data.fd = fd;

    int ret = epoll_ctl(m_handle, EPOLL_CTL_MOD, fd, &event);

    if (ret < 0) {
        e.assign(errno, std::system_category());
        return false;
    } else {
        e.assign(0, std::system_category());
        return true;
    }
}

bool net::epoll::remove(socket::native_handle_type fd) noexcept
{
    int ret = epoll_ctl(m_handle, EPOLL_CTL_DEL, fd, nullptr);
    if (ret < 0)
        return false;
    else {
        --size;
        return true;
    }
}

bool net::epoll::remove(socket::native_handle_type fd, std::error_code& e) noexcept
{
    int ret = epoll_ctl(m_handle, EPOLL_CTL_DEL, fd, nullptr);

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
    std::error_code e;
    auto result = execute(timeout, e);
    if (e) throw std::system_error(e);
    return result;
}

size_t net::epoll::execute(std::optional<std::chrono::milliseconds> timeout, std::error_code& e)
{
    data.clear();
    data.resize(size);
    int ret = epoll_wait(m_handle, data.data(), size, timeout ? timeout->count() : -1);
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
    std::error_code e;
    auto result = execute(timeout, sigmask, e);
    if (e) throw std::system_error(e);
    return result;
}

size_t net::epoll::execute(std::optional<std::chrono::milliseconds> timeout, const sigset_t& sigmask, std::error_code& e)
{
    data.clear();
    data.resize(size);
    int ret = epoll_pwait(m_handle, data.data(), size, timeout ? timeout->count() : -1, &sigmask);
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
    if (::close(m_handle) < 0) {
        throw std::system_error(errno, std::system_category());
    } else {
        m_handle = -1;
    }
}

void net::epoll::close(std::error_code& e) noexcept
{
    if (::close(m_handle) < 0)
        e.assign(errno, std::system_category());
    else {
        e.assign(0, std::system_category());
        m_handle = -1;
    }
}

#endif

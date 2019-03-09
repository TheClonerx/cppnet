#pragma once
#include <chrono>
#include <optional>
#include <system_error>
#include <list>

namespace net {

struct select_return_t
{
    size_t reads, writes, exceptions;
};

class select {
public:

    enum class EventFlags : unsigned
    {
        read = 1,
        write = 2,
        except = 4
    };

    bool add(int, EventFlags);
    bool remove(int, EventFlags);

    select_return_t execute(std::optional<std::chrono::microseconds>);
    select_return_t execute(std::optional<std::chrono::microseconds>, std::error_code&) noexcept;

#ifdef _GNU_SOURCE
    select_return_t execute(std::optional<std::chrono::nanoseconds>, const sigset_t&);
    select_return_t execute(std::optional<std::chrono::nanoseconds>, const sigset_t&, std::error_code&) noexcept;
#endif

    // ranges

    template <typename It>
    It get(It start, It stop, EventFlags events) const noexcept(noexcept(*start = 0) && noexcept(++start == stop))
    {

        for (const auto& item : fdlist)
        {
            if (start == stop) return stop;
            if (
                (static_cast<unsigned>(events) & static_cast<unsigned>(EventFlags::read) && static_cast<unsigned>(item.sevents) & static_cast<unsigned>(EventFlags::read)) ||
                (static_cast<unsigned>(events) & static_cast<unsigned>(EventFlags::write) && static_cast<unsigned>(item.sevents) & static_cast<unsigned>(EventFlags::write)) ||
                (static_cast<unsigned>(events) & static_cast<unsigned>(EventFlags::except) && static_cast<unsigned>(item.sevents) & static_cast<unsigned>(EventFlags::except))
            )
            {
                *start = item.fd;
                ++start;
            }
        }
        return start;
    }

    // inserters

    template <typename It>
    void get(It start, EventFlags events) const noexcept(noexcept(*start = 0) && noexcept(++start))
    {
        for (const auto& item : fdlist)
        {
            if (
                (static_cast<unsigned>(events) & static_cast<unsigned>(EventFlags::read) && static_cast<unsigned>(item.sevents) & static_cast<unsigned>(EventFlags::read)) ||
                (static_cast<unsigned>(events) & static_cast<unsigned>(EventFlags::write) && static_cast<unsigned>(item.sevents) & static_cast<unsigned>(EventFlags::write)) ||
                (static_cast<unsigned>(events) & static_cast<unsigned>(EventFlags::except) && static_cast<unsigned>(item.sevents) & static_cast<unsigned>(EventFlags::except))
            )
            {
                *start = item.fd;
                ++start;
            }
        }
        return start;
    }


private:
    struct Item
    {
        int fd;
        EventFlags events; // events to select
        EventFlags sevents; // selected events
    }; 

    std::list<Item> fdlist;
};
}

inline net::select::EventFlags operator|(net::select::EventFlags lhs, net::select::EventFlags rhs)
{
    return static_cast<net::select::EventFlags>(static_cast<unsigned>(lhs) | static_cast<unsigned>(rhs));
}

inline net::select::EventFlags operator&(net::select::EventFlags lhs, net::select::EventFlags rhs)
{
    return static_cast<net::select::EventFlags>(static_cast<unsigned>(lhs) & static_cast<unsigned>(rhs));
}

inline net::select::EventFlags operator^(net::select::EventFlags lhs, net::select::EventFlags rhs)
{
    return static_cast<net::select::EventFlags>(static_cast<unsigned>(lhs) ^ static_cast<unsigned>(rhs));
}

inline bool operator!(net::select::EventFlags rhs)
{
    return !static_cast<unsigned>(rhs);
}

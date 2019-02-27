#pragma once
#include <chrono>
#include <optional>
#include <system_error>
#include <tuple>
#include <vector>

namespace net {
class select {
public:
    bool add_read(int);
    bool add_write(int);
    bool add_except(int);

    bool remove_read(int);
    bool remove_write(int);
    bool remove_except(int);

    std::tuple<size_t, size_t, size_t> execute(std::optional<std::chrono::microseconds>);
    std::tuple<size_t, size_t, size_t> execute(std::optional<std::chrono::microseconds>, std::error_code&) noexcept;

#ifdef _GNU_SOURCE
    std::tuple<size_t, size_t, size_t> execute(std::optional<std::chrono::nanoseconds>, const sigset_t&);
    std::tuple<size_t, size_t, size_t> execute(std::optional<std::chrono::nanoseconds>, const sigset_t&, std::error_code&) noexcept;
#endif

    // ranges

    template <typename It>
    void get_read(It start, It end) const
    {
        for (const auto& t : fdlist) {
            if (std::get<1>(t) == Type::READ && std::get<2>(t)) {
                *start = std::get<0>(t);
                ++start;
                if (start == end)
                    return;
            }
        }
    }

    template <typename It>
    void get_write(It start, It end) const
    {
        for (const auto& t : fdlist) {
            if (std::get<1>(t) == Type::WRITE && std::get<2>(t)) {
                *start = std::get<0>(t);
                ++start;
                if (start == end)
                    return;
            }
        }
    }
    template <typename It>
    void get_except(It start, It end) const
    {
        for (const auto& t : fdlist) {
            if (std::get<1>(t) == Type::EXCEPT && std::get<2>(t)) {
                *start = std::get<0>(t);
                ++start;
                if (start == end)
                    return;
            }
        }
    }

    // inserters

    template <typename It>
    void get_read(It start) const
    {
        for (const auto& t : fdlist) {
            if (std::get<1>(t) == Type::READ && std::get<2>(t)) {
                *start = std::get<0>(t);
                ++start;
            }
        }
    }
    template <typename It>
    void get_write(It start) const
    {
        for (const auto& t : fdlist) {
            if (std::get<1>(t) == Type::WRITE && std::get<2>(t)) {
                *start = std::get<0>(t);
                ++start;
            }
        }
    }
    template <typename It>
    void get_except(It start) const
    {
        for (const auto& t : fdlist) {
            if (std::get<1>(t) == Type::EXCEPT && std::get<2>(t)) {
                *start = std::get<0>(t);
                ++start;
            }
        }
    }

private:
    enum Type {
        READ,
        WRITE,
        EXCEPT
    };
    std::vector<std::tuple<int, Type, bool>> fdlist;
};
}
#pragma once
#include <string_view>
#include <system_error>
#include <utility>

#ifdef _WIN32
#include <WinSock2.h>
#ifndef CPPNET_IMPL
#include <cppnet/wsa_init.hpp>
#endif
#else
#include <netdb.h>
#include <sys/socket.h>
#endif

#include <cppnet/address.hpp>

#if __cplusplus >= 202002L
#include <version>
#ifdef __cpp_lib_three_way_comparison
#include <compare>
#endif
#endif

namespace net {

class socket {
public:
#ifdef _WIN32
    using native_handle_type = SOCKET;
    static constexpr native_handle_type invalid_handle = INVALID_SOCKET;
#else
    using native_handle_type = int;
    static constexpr native_handle_type invalid_handle = -1;
#endif

    constexpr socket() noexcept = default;

    constexpr socket(socket&& rhs) noexcept
        : m_handle { rhs.m_handle }
    {
        rhs.m_handle = invalid_handle;
    }

    constexpr socket& operator=(socket&& rhs) noexcept
    {
        m_handle = rhs.m_handle;
        rhs.m_handle = invalid_handle;
        return *this;
    }

    ~socket() noexcept;

    socket(int family, int type, int protocol);
    socket(int family, int type, int protocol, std::error_code&) noexcept;

private:
    static constexpr struct from_native_handle_t {
    } from_native_handle {};
    constexpr socket(from_native_handle_t, native_handle_type handle) noexcept
        : m_handle { handle }
    {
    }

public:
    static std::pair<socket, socket> pair(int family = AF_UNIX, int type = SOCK_STREAM, int protocol = 0);
    static std::pair<socket, socket> pair(int family, int type, int protocol, std::error_code&) noexcept;

    socket dup() const;
    socket dup(std::error_code&) const noexcept;

    size_t recv(void* buffer, size_t buffer_size, int flags = 0);
    size_t recv(void* buffer, size_t buffer_size, int flags, std::error_code&) noexcept;

    size_t recvfrom(void* buffer, size_t buffer_size, int flags, sockaddr* address, size_t* address_size);
    size_t recvfrom(void* buffer, size_t buffer_size, int flags, sockaddr* address, size_t* address_size, std::error_code&) noexcept;

    size_t recvfrom(void* buffer, size_t buffer_size, int flags, address& addr)
    {
        return recvfrom(buffer, buffer_size, flags, reinterpret_cast<sockaddr*>(&addr.m_socket_address), &addr.m_socket_address_size);
    }

    size_t recvfrom(void* buffer, size_t buffer_size, int flags, address& addr, std::error_code& e) noexcept
    {
        return recvfrom(buffer, buffer_size, flags, reinterpret_cast<sockaddr*>(&addr.m_socket_address), &addr.m_socket_address_size, e);
    }

    size_t send(const void* buffer, size_t buffer_size, int flags = 0);
    size_t send(const void* buffer, size_t buffer_size, int flags, std::error_code&) noexcept;

    size_t send(std::string_view buffer, int flags = 0)
    {
        return send(buffer.data(), buffer.size(), flags);
    }
    size_t send(std::string_view buffer, int flags, std::error_code& e) noexcept
    {
        return send(buffer.data(), buffer.size(), flags, e);
    }

    size_t sendto(const void* buffer, size_t buffer_size, int flags, const sockaddr* address, size_t address_size);
    size_t sendto(const void* buffer, size_t buffer_size, int flags, const sockaddr* address, size_t address_size, std::error_code&) noexcept;

    size_t sendto(const void* buffer, size_t buffer_size, int flags, address const& addr)
    {
        return sendto(buffer, buffer_size, flags, reinterpret_cast<sockaddr const*>(&addr.m_socket_address), addr.m_socket_address_size);
    }

    size_t sendto(const void* buffer, size_t buffer_size, int flags, address const& addr, std::error_code& e) noexcept
    {
        return sendto(buffer, buffer_size, flags, reinterpret_cast<sockaddr const*>(&addr.m_socket_address), addr.m_socket_address_size, e);
    }

    void connect(const sockaddr* address, size_t address_size);
    void connect(const sockaddr* address, size_t address_size, std::error_code&) noexcept;

    void connect(const address& addr)
    {
        connect(reinterpret_cast<const sockaddr*>(&addr.m_socket_address), addr.m_socket_address_size);
    }

    void connect(const address& addr, std::error_code& e) noexcept
    {
        connect(reinterpret_cast<const sockaddr*>(&addr.m_socket_address), addr.m_socket_address_size, e);
    }

    socket accept(sockaddr* address, size_t* address_size);
    socket accept(sockaddr* address, size_t* address_size, std::error_code&) noexcept;

    socket accept()
    {
        return accept(nullptr, nullptr);
    }

    socket accept(std::error_code& e) noexcept
    {
        return accept(nullptr, nullptr, e);
    }

    socket accept(address& addr)
    {
        return accept(reinterpret_cast<sockaddr*>(&addr.m_socket_address), &addr.m_socket_address_size);
    }

    socket accept(address& addr, std::error_code& e) noexcept
    {
        return accept(reinterpret_cast<sockaddr*>(&addr.m_socket_address), &addr.m_socket_address_size, e);
    }

    void listen(int backlog);
    void listen(int backlog, std::error_code&) noexcept;

    void bind(const sockaddr* address, size_t address_size);
    void bind(const sockaddr* address, size_t address_size, std::error_code&) noexcept;

    void bind(const address& addr, std::error_code& e) noexcept
    {
        bind(reinterpret_cast<const sockaddr*>(&addr.m_socket_address), addr.m_socket_address_size, e);
    }

    void bind(const address& addr)
    {
        bind(reinterpret_cast<const sockaddr*>(&addr.m_socket_address), addr.m_socket_address_size);
    }

    net::address getsockname(std::error_code&) noexcept;
    net::address getsockname();

    net::address getpeername(std::error_code&) noexcept;
    net::address getpeername();

    void getsockopt(int level, int optname, void* optval, size_t* optlen) const;
    void getsockopt(int level, int optname, void* optval, size_t* optlen, std::error_code&) const noexcept;

    void setsockopt(int level, int optname, const void* optval, size_t optlen, std::error_code&) noexcept;
    void setsockopt(int level, int optname, const void* optval, size_t optlen);

    void setblocking(bool);
    void setblocking(bool, std::error_code&) noexcept;

    template <typename T, std::enable_if_t<std::is_trivially_copyable_v<T>, std::nullptr_t> = nullptr>
    T getsockopt(int level, int optname) const
    {
        size_t optlen = sizeof(T);
        if constexpr (std::is_trivially_default_constructible_v<T>) { // it may be easier to the compiler to optimize this
            std::remove_cv_t<T> optval;
            getsockopt(level, optname, &optval, &optlen);
            return optval;
        } else {
            std::remove_cv_t<T> optval[1];
            getsockopt(level, optname, optval, &optlen);
            return optval[0];
        }
    }

    template <typename T, std::enable_if_t<std::is_trivially_copyable_v<T>, std::nullptr_t> = nullptr>
    T getsockopt(int level, int optname, std::error_code& e) const noexcept
    {
        size_t optlen = sizeof(T);
        if constexpr (std::is_trivially_default_constructible_v<T>) { // it may be easier to the compiler to optimize this
            std::remove_cv_t<T> optval;
            getsockopt(level, optname, &optval, &optlen, e);
            return optval;
        } else {
            std::remove_cv_t<T> optval[1];
            getsockopt(level, optname, optval, &optlen, e);
            return optval[0];
        }
    }

    template <typename T, std::enable_if_t<std::is_trivially_copyable_v<T>, std::nullptr_t> = nullptr>
    void setsockopt(int level, int optname, T&& optval)
    {
        setsockopt(level, optname, &optval, sizeof(T));
    }

    template <typename T, std::enable_if_t<std::is_trivially_copyable_v<T>, std::nullptr_t> = nullptr>
    void setsockopt(int level, int optname, T&& optval, std::error_category& e) noexcept
    {
        setsockopt(level, optname, &optval, sizeof(T), e);
    }

    constexpr native_handle_type native_handle() const noexcept
    {
        return m_handle;
    }

    int family() const;
    int family(std::error_code&) const noexcept;

    int type() const;
    int type(std::error_code&) const noexcept;

    int protocol() const;
    int protocol(std::error_code&) const noexcept;

    bool accepting() const;
    bool accepting(std::error_code&) const noexcept;

    void shutdown(int way);
    void shutdown(int way, std::error_code&) noexcept;

    void close();
    void close(std::error_code&) noexcept;

    std::error_code error() const noexcept
    {
        auto error_number = getsockopt<int>(SOL_SOCKET, SO_ERROR);
        return std::error_code { error_number, std::system_category() };
    }

    friend void swap(socket& lhs, socket& rhs) noexcept
    {
        using std::swap;
        swap(lhs.m_handle, rhs.m_handle);
    }

#ifndef __cpp_lib_three_way_comparison
#define SOCKET_COMPARATION(op)                                   \
    constexpr bool operator op(socket const& rhs) const noexcept \
    {                                                            \
        return m_handle op rhs.m_handle;                         \
    }
    SOCKET_COMPARATION(==)
    SOCKET_COMPARATION(!=)
    SOCKET_COMPARATION(<)
    SOCKET_COMPARATION(>)
    SOCKET_COMPARATION(<=)
    SOCKET_COMPARATION(>=)
#undef SOCKET_COMPARATION
#else
    constexpr bool operator==(socket const& rhs) const noexcept
    {
        return m_handle == rhs.m_handle;
    }

    constexpr std::partial_ordering operator<=>(socket const& rhs) const noexcept
    {
        if (m_handle == rhs.m_handle && rhs.m_handle == invalid_handle)
            return std::partial_ordering::unordered;
        else
            return m_handle <=> rhs.m_handle;
    }
#endif

    explicit constexpr operator bool() const noexcept
    {
        return m_handle != invalid_handle;
    }

protected:
    native_handle_type m_handle = invalid_handle;
};

} // namespace net

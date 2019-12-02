#pragma once
#include <string_view>
#include <system_error>
#include <utility>

#ifdef _WIN32
#include <WinSock2.h>
#else
#include <netdb.h>
#include <sys/socket.h>
#endif

#include <net/address.hpp>

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

    socket() noexcept = default;

    socket(socket&&) noexcept;
    socket& operator=(socket&&) noexcept;

    socket(const socket&) = delete;
    socket& operator=(const socket&) = delete;

    ~socket() noexcept;

    socket(int family, int type, int protocol);
    socket(int family, int type, int protocol, std::error_code&) noexcept;

private:
    static constexpr struct from_native_handle_t {
    } from_native_handle {};
    socket(from_native_handle_t, native_handle_type);

public:
    static std::pair<socket, socket> pair(int family = AF_UNIX, int type = SOCK_STREAM, int protocol = 0);
    static std::pair<socket, socket> pair(int family, int type, int protocol, std::error_code&) noexcept;

    socket dup() const;
    socket dup(std::error_code&) const noexcept;

    size_t recv(void* buffer, size_t buffer_size, int flags = 0);
    size_t recv(void* buffer, size_t buffer_size, int flags, std::error_code&) noexcept;

    size_t recvfrom(void* buffer, size_t buffer_size, int flags, sockaddr* address, size_t* address_size);
    size_t recvfrom(void* buffer, size_t buffer_size, int flags, sockaddr* address, size_t* address_size, std::error_code&) noexcept;

    inline size_t recvfrom(void* buffer, size_t buffer_size, int flags, address& addr)
    {
        return recvfrom(buffer, buffer_size, flags, reinterpret_cast<sockaddr*>(&addr.m_socket_address), &addr.m_socket_address_size);
    }

    inline size_t recvfrom(void* buffer, size_t buffer_size, int flags, address& addr, std::error_code& e) noexcept
    {
        return recvfrom(buffer, buffer_size, flags, reinterpret_cast<sockaddr*>(&addr.m_socket_address), &addr.m_socket_address_size, e);
    }

    size_t send(const void* buffer, size_t buffer_size, int flags = 0);
    size_t send(const void* buffer, size_t buffer_size, int flags, std::error_code&) noexcept;

    size_t send(std::string_view buffer, int flags = 0);
    size_t send(std::string_view buffer, int flags, std::error_code&) noexcept;

    size_t sendto(const void* buffer, size_t buffer_size, int flags, const sockaddr* address, size_t address_size);
    size_t sendto(const void* buffer, size_t buffer_size, int flags, const sockaddr* address, size_t address_size, std::error_code&) noexcept;

    inline size_t sendto(const void* buffer, size_t buffer_size, int flags, address& addr)
    {
        return sendto(buffer, buffer_size, flags, reinterpret_cast<sockaddr*>(&addr.m_socket_address), addr.m_socket_address_size);
    }

    inline size_t sendto(const void* buffer, size_t buffer_size, int flags, address& addr, std::error_code& e) noexcept
    {
        return sendto(buffer, buffer_size, flags, reinterpret_cast<sockaddr*>(&addr.m_socket_address), addr.m_socket_address_size, e);
    }

    void connect(const sockaddr* address, size_t address_size);
    void connect(const sockaddr* address, size_t address_size, std::error_code&) noexcept;

    inline void connect(const address& addr)
    {
        connect(reinterpret_cast<const sockaddr*>(&addr.m_socket_address), addr.m_socket_address_size);
    }

    inline void connect(const address& addr, std::error_code& e) noexcept
    {
        connect(reinterpret_cast<const sockaddr*>(&addr.m_socket_address), addr.m_socket_address_size, e);
    }

    socket accept(sockaddr* address, size_t* address_size);
    socket accept(sockaddr* address, size_t* address_size, std::error_code&) noexcept;

    inline socket accept()
    {
        return accept(nullptr, nullptr);
    }

    inline socket accept(std::error_code& e) noexcept
    {
        return accept(nullptr, nullptr, e);
    }

    inline socket accept(address& addr)
    {
        return accept(reinterpret_cast<sockaddr*>(&addr.m_socket_address), &addr.m_socket_address_size);
    }

    inline socket accept(address& addr, std::error_code& e) noexcept
    {
        return accept(reinterpret_cast<sockaddr*>(&addr.m_socket_address), &addr.m_socket_address_size, e);
    }

    void listen(int backlog);
    void listen(int backlog, std::error_code&) noexcept;

    void bind(const sockaddr* address, size_t address_size);
    void bind(const sockaddr* address, size_t address_size, std::error_code&) noexcept;

    inline void bind(const address& addr, std::error_code& e) noexcept
    {
        bind(reinterpret_cast<const sockaddr*>(&addr.m_socket_address), addr.m_socket_address_size, e);
    }

    inline void bind(const address& addr)
    {
        bind(reinterpret_cast<const sockaddr*>(&addr.m_socket_address), addr.m_socket_address_size);
    }

    void getsockopt(int level, int optname, void* optval, size_t* optlen) const;
    void getsockopt(int level, int optname, void* optval, size_t* optlen, std::error_code&) const noexcept;

    void setsockopt(int level, int optname, const void* optval, size_t optlen, std::error_code&) noexcept;
    void setsockopt(int level, int optname, const void* optval, size_t optlen);

    void setblocking(bool);
    void setblocking(bool, std::error_code&) noexcept;

    template <typename T>
    T getsockopt(int level, int optname) const
    {
        size_t optlen = sizeof(T);
        // we don't want to default-initialize T
        // (T optval; getsockopt(level, optname, &optval, &optlen);)
        // so we just create a stack buffer
        char optval[sizeof(T)];
        getsockopt(level, optname, optval, &optlen);
        return *((T*)optval);
    }

    template <typename T>
    T getsockopt(int level, int optname, std::error_code& e) const noexcept
    {
        size_t optlen = sizeof(T);
        // we don't want to default-initialize T
        // (T optval; getsockopt(level, optname, &optval, &optlen);)
        // so we just create a stack buffer and move it later
        char optval[sizeof(T)];
        getsockopt(level, optname, optval, &optlen, e);
        return *((T*)optval);
    }

    template <typename T>
    void setsockopt(int level, int optname, T&& optval)
    {
        setsockopt(level, optname, &optval, sizeof(T));
    }

    template <typename T>
    void setsockopt(int level, int optname, T&& optval, std::error_category& e) noexcept
    {
        setsockopt(level, optname, &optval, sizeof(T), e);
    }

    native_handle_type native_handle() noexcept;

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

    friend void swap(socket& lhs, socket& rhs) noexcept
    {
        using std::swap;
        swap(lhs.m_Handle, rhs.m_Handle);
    }

protected:
    native_handle_type m_Handle = invalid_handle;
};
} // namespace net

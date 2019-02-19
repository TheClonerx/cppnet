#if __cplusplus < 201103L
#error
#endif

#pragma once
#include <memory>
#include <string>
#include <string_view>
#include <system_error>

#include <netdb.h>
#include <sys/socket.h>

#if __cplusplus >= 201703L
#include <string_view>
#endif

namespace net {

struct any_addr_t {
};
constexpr any_addr_t any_addr;

struct localhost_t {
};
constexpr localhost_t localhost;


class socket {
public:
    socket(); // may trow
    socket(socket&&) noexcept = default;
    socket& operator=(socket&&) noexcept = default;
    socket(const socket&) = delete;
    socket& operator=(const socket&) = delete;
    ~socket() noexcept;

    explicit socket(std::error_code&) noexcept;

    socket(int, int, int);
    socket(int, int, int, std::error_code&) noexcept;

    static socket from_fileno(int) noexcept;

    socket dup() const;
    socket dup(std::error_code&) const noexcept;

    size_t recv(void*, size_t, int = 0);
    size_t recv(void*, size_t, int, std::error_code&) noexcept;

    size_t send(void*, size_t, int = 0);
    size_t send(void*, size_t, int, std::error_code&) noexcept;

#if __cplusplus >= 201703L
    size_t send(std::string_view, int = 0);
    size_t send(std::string_view, int, std::error_code&) noexcept;
#endif

    void connect(const sockaddr*, socklen_t);
    void connect(const sockaddr*, socklen_t, std::error_code&) noexcept;

    // begin ipv4
    void connect(const char*, uint16_t);
    void connect(const char*, uint16_t, std::error_code&) noexcept;

#if __cplusplus >= 201703L
    void connect(std::string_view, uint16_t);
    void connect(std::string_view, uint16_t, std::error_code&) noexcept;
#endif
    // end ipv4

    socket accept();
    socket accept(std::error_code&) noexcept;

    socket accept(int); // for accept4
    socket accept(int, std::error_code&) noexcept;

    socket accept(sockaddr *addr, socklen_t addrlen);
    socket accept(sockaddr *addr, socklen_t addrlen, std::error_code&) noexcept;

    socket accept(sockaddr*, socklen_t, int); // for accept4
    socket accept(sockaddr*, socklen_t, int, std::error_code&) noexcept;

    void listen(int);
    void listen(int, std::error_code&) noexcept;

    void bind(const sockaddr*, socklen_t);
    void bind(const sockaddr*, socklen_t, std::error_code&) noexcept;

    // begin ipv4
    // NOTE: const char* and std::string_view variants may works under ipv6,
    //       since they work with getaddrinfo (resolving to an actual ipv6 address).
    void bind(const char*, uint16_t);
    void bind(const char*, uint16_t, std::error_code&) noexcept;

#if __cplusplus >= 201703L
    void bind(std::string_view, uint16_t);
    void bind(std::string_view, uint16_t, std::error_code&) noexcept;
#endif

    void bind(any_addr_t, uint16_t);
    void bind(any_addr_t, uint16_t, std::error_code&) noexcept;

    void bind(localhost_t, uint16_t);
    void bind(localhost_t, uint16_t, std::error_code&) noexcept;
    // end ipv4

    void getsockopt(int, int, void*, socklen_t*) const;
    void getsockopt(int, int, void*, socklen_t*, std::error_code&) const noexcept;

    void setsockopt(int, int, void*, socklen_t, std::error_code&) noexcept;
    void setsockopt(int, int, void*, socklen_t);

    void setblocking(bool);
    void setblocking(bool, std::error_code&) noexcept;

    template <typename T>
    T getsockopt(int level, int optname) const
    {
        socklen_t optlen = sizeof(T);
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
        socklen_t optlen = sizeof(T);
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

    int fileno() const noexcept;

    int family() const;
    int family(std::error_code&) const noexcept;

    int type() const;
    int type(std::error_code&) const noexcept;

    int protocol() const;
    int protocol(std::error_code&) const noexcept;

    bool is_accepting() const;
    bool is_accepting(std::error_code&) const noexcept;

protected:
    int fd;
};
} // namespace net

#ifdef _WIN32
#include <Winsock2.h>
#else
#include <unistd.h>
#endif
namespace net::impl {
#ifdef _WIN32
	using socket_handle = SOCKET;
	using socklen_t = int;
	using ssize_t = ptrdiff_t;
#else
	using socket_handle = int;
	using socklen_t = ::socklen_t;
	using ssize_t = ::ssize_t;
#endif
}
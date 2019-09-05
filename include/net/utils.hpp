#pragma once
#include <string>
#include <system_error>

#ifdef _WIN32
#include <Winsock2.h>
#else
#include <sys/socket.h>
#endif

namespace net {
std::string addr_to_str(const sockaddr_storage* address);
std::string addr_to_str(const sockaddr_storage* address, std::error_code&) noexcept;
}

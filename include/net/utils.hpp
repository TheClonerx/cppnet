#pragma once
#include <string>
#include <sys/socket.h>
#include <system_error>

namespace net {
std::string addr_to_str(const sockaddr_storage* address);
std::string addr_to_str(const sockaddr_storage* address, std::error_code&) noexcept;
}

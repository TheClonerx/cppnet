#include <iostream>
#include <vector>
#include <string>

#include <cppnet/getaddrinfo.hpp>

#ifndef _WIN32
#include <arpa/inet.h>
#endif

std::string addr_to_str(const sockaddr_storage* addr); // browser friendly address

std::ostream& operator<<(std::ostream& os, net::address const& addr)
{
    os << addr_to_str(addr.address_pointer());
    return os;
}

int main(int argc, const char* argv[]) try {
    if (argc < 2 || argc > 3) {
        std::cerr << "Usage:\n\t" << argv[0] << " host [service/port]\n";
        exit(EXIT_FAILURE);
    }

    const char* service = (argc == 3) ? argv[2] : nullptr;

    std::vector<net::address_info> ainfos;

    net::getaddrinfo(std::back_inserter(ainfos), argv[1], service, 0, 0, 0, AI_CANONNAME);

    std::cout << "Found " << ainfos.size() << " address(es):\n";
    for (const net::address_info& ainfo : ainfos) {
        std::cout << "\tFamily:   " << ainfo.family()
                  << "\tType:     " << ainfo.type()
                  << "\tProtocol: " << ainfo.protocol()
                  << "\tAddress:  " << ainfo.address();

        if (!ainfo.canon_name().empty())
            std::cout << "\tCanon name: " << ainfo.canon_name();
        std::cout << '\n';
    }
} catch (std::system_error& e) {
    std::cerr << '\n' << e.code().category().name()
              << " error (" << e.code().value() << "):\n\t"
              << e.what() << '\n';
} catch (std::exception& e) {
    std::cerr << "\nstd exception:\n\t" << e.what() << '\n';
}

std::string addr_to_str(const sockaddr_storage* addr) // browser friendly address
{
    using namespace std::literals;
    std::string buffer(64, '\0');
    if (addr->ss_family == AF_INET) {
        auto in_addr = reinterpret_cast<const sockaddr_in*>(addr);
        inet_ntop(AF_INET, &in_addr->sin_addr, std::data(buffer), std::size(buffer));
    } else if (addr->ss_family == AF_INET6) {
        auto in6_addr = reinterpret_cast<const sockaddr_in6*>(addr);
        inet_ntop(AF_INET6, &in6_addr->sin6_addr, std::data(buffer), std::size(buffer));
    } else {
        buffer = "???";
    }
    buffer.resize(std::strlen(buffer.c_str()));
    if (auto in_addr = reinterpret_cast<sockaddr_in const*>(addr); in_addr->sin_port) {
        if (addr->ss_family == AF_INET6) {
            buffer = "["s + buffer;
            buffer += "]"sv;
        }
        buffer += ":"sv;
        buffer += std::to_string(htons(in_addr->sin_port));
    }
    return buffer;
}

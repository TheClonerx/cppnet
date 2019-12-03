#include <iostream>
#include <vector>

#include <net/getaddrinfo.hpp>
#include <arpa/inet.h>

std::string addr_to_str(const sockaddr* addr); // browser friendly address

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
                  << "\tAddress:  " << addr_to_str(ainfo.address().address_pointer());

        if (!ainfo.canon_name().empty())
            std::cout << "\tCanon name: " << ainfo.canon_name();
        std::cout << '\n';
    }
} catch (std::system_error& e) {
    std::cerr << e.code().category().name()
              << " error (" << e.code().value() << "):\n\t"
              << e.what() << '\n';
} catch (std::exception& e) {
    std::cerr << "std exception:\n\t" << e.what() << '\n';
}

std::string addr_to_str(const sockaddr* addr) // browser friendly address
{
    char buffer[64] { 0 };
    if (addr->sa_family == AF_INET) {
        auto in_addr = reinterpret_cast<const sockaddr_in*>(addr);
        inet_ntop(AF_INET, &in_addr->sin_addr, buffer, std::size(buffer));
        return buffer + (':' + std::to_string(ntohs(in_addr->sin_port)));
    } else if (addr->sa_family == AF_INET6) {
        auto in6_addr = reinterpret_cast<const sockaddr_in6*>(addr);
        inet_ntop(AF_INET6, &in6_addr->sin6_addr, buffer, std::size(buffer));
        return "[" + (buffer + ("]:" + std::to_string(ntohs(in6_addr->sin6_port))));
    }
    return "???";
}
#include <iostream>
#include <string>
#include <vector>

#include <cppnet/getaddrinfo.hpp>

int main(int argc, const char *argv[])
try {
    if (argc < 2 || argc > 3) {
        std::cerr << "Usage:\n\t" << argv[0] << " host [service/port]\n";
        exit(EXIT_FAILURE);
    }

    const char *service = (argc == 3) ? argv[2] : nullptr;

    std::vector<net::address_info> ainfos;

    net::getaddrinfo(std::back_inserter(ainfos), argv[1], service, 0, 0, 0, AI_CANONNAME);

    std::cout << "Found " << ainfos.size() << " address(es):\n";
    for (const net::address_info &ainfo : ainfos) {
        std::cout << "\tFamily:   " << ainfo.family()
                  << "\tType:     " << ainfo.type()
                  << "\tProtocol: " << ainfo.protocol()
                  << "\tAddress:  " << ainfo.address();

        if (!ainfo.canon_name().empty())
            std::cout << "\tCanonical name: " << ainfo.canon_name();
        std::cout << '\n';
    }
} catch (std::system_error &e) {
    std::cerr << '\n'
              << e.code().category().name()
              << " error (" << e.code().value() << "):\n\t"
              << e.what() << '\n';
} catch (std::exception &e) {
    std::cerr << "\nstd exception:\n\t" << e.what() << '\n';
}

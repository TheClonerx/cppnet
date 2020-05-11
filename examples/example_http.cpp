#include <cppnet/getaddrinfo.hpp>
#include <cppnet/socket.hpp>

#include <iostream>
#include <string_view>
#include <string>
#include <charconv>

using namespace std::literals;

constexpr auto request =
    "GET / HTTP/1.1\r\n"
    "Host: example.com\r\n"
    "\r\n"sv;

int main(int argc, const char** argv) try {

    size_t max_bytes = 1024;

    if (argc == 2) {
        if (argv[1] == "-h"sv || argv[1] == "--help"sv) {
            std::cout << "Usage:\n\t" << argv[0] << " [max bytes]\n\n";
            return EXIT_SUCCESS;
        }
        auto [ptr, ec] = std::from_chars(argv[1], argv[1] + std::strlen(argv[1]), max_bytes);
        if (ec != std::errc{}) {
            std::cout << "Usage:\n\t" << argv[0] << " [max bytes]\n\n";
            return EXIT_FAILURE;
        }
    }

    std::clog << "Resolving..." << std::endl;
    net::address_info ainfo = net::getaddrinfo("example.com", "80", AF_UNSPEC, SOCK_STREAM);
    net::socket sock { ainfo.family(), ainfo.type(), ainfo.protocol() };

    std::clog << "Connecting..." << std::endl;
    sock.connect(ainfo.address());

    std::clog << "Sending request..." << std::endl;
    sock.send(request);

    std::clog << "Receiving response..." << std::endl;
    std::string buff(max_bytes, '\0');
    buff.resize(sock.recv(buff.data(), buff.size()));

    std::clog << "Done.\n" << std::endl;

    std::cout << buff << std::endl;

} catch (std::system_error& e) {
    std::cerr << e.code().category().name()
              << " error (" << e.code().value() << "):\n\t"
              << e.what() << '\n';
} catch (std::exception& e) {
    std::cerr << "std exception:\n\t" << e.what() << '\n';
}

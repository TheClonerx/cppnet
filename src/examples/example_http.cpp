#include "net/getaddrinfo.hpp"
#include "net/socket.hpp"

#include <iostream>
#include <string_view>

using namespace std::literals;

constexpr auto request = 
    "GET /manual/5.3/contents.html HTTP/1.1\r\n"
    "Host: www.lua.org\r\n"
    "\r\n"sv;

int main() try {

    std::cout << "Resolving..." << std::endl;
    net::address_info ainfo = net::getaddrinfo("www.lua.org", "80");
    net::socket sock { ainfo.family(), ainfo.type(), ainfo.protocol() };

    std::cout << "Connecting..." << std::endl;
    sock.connect(ainfo.address());
    std::cout << "Connected." << std::endl;

    std::cout << "Sending request..." << std::endl;
    sock.send(request);
    std::cout << "Done.\n" << std::endl;

    std::string buff(1024, '\0');
    buff.resize(sock.recv(buff.data(), buff.size()));

    std::cout << buff << std::endl;

} catch (std::system_error& e) {
    std::cerr << e.code().category().name()
              << " error (" << e.code().value() << "):\n\t"
              << e.what() << '\n';
} catch (std::exception& e) {
    std::cerr << "std exception:\n\t" << e.what() << '\n';
}
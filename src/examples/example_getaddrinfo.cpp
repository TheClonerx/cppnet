#include <iostream>
#include <list>
#include <mutex>
#include <thread>
#include <vector>

#include "net/getaddrinfo.hpp"
//#include "net/utils.hpp"

std::mutex cerr_mutex;

void test_back_inserter(std::back_insert_iterator<std::list<net::address_info>>, const char*, const char*);
void test_range(const char*, const char*);
void test_reference(const char*, const char*);
void test_return(const char*, const char*);

#include <arpa/inet.h>

std::string addr_to_str(const sockaddr* addr)
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
    return "?";
}

int main(int argc, const char* argv[]) try {
    if (argc < 2 || argc > 3) {
        std::cerr << "Usage:\n\t" << argv[0] << " host [service/port]\n";
        exit(EXIT_FAILURE);
    }

    const char* service = (argc == 3) ? argv[2] : nullptr;

    std::list<net::address_info> ainfos;

    std::thread t1(test_back_inserter, std::back_inserter(ainfos), argv[1], service);
    std::thread t2(test_range, argv[1], service);
    std::thread t3(test_reference, argv[1], service);
    std::thread t4(test_return, argv[1], service);

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    if (ainfos.empty())
        return EXIT_FAILURE;

    std::cout << "Found " << ainfos.size() << " address(es):\n";
    for (const net::address_info& ainfo : ainfos) {
        std::cout << "\tFamily: " << ainfo.family()
                  << "\tType: " << ainfo.type()
                  << "\tProtocol: " << ainfo.protocol()
                  << "\tAddress: " << addr_to_str(ainfo.address().address_pointer());
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

void test_back_inserter(std::back_insert_iterator<std::list<net::address_info>> it, const char* host, const char* service)
{
    std::error_code e;
    net::getaddrinfo(it, host, service, 0, 0, 0, AI_CANONNAME, e);
    if (e) {
        std::lock_guard<std::mutex> guard(cerr_mutex);
        std::cerr << "test_back_inserter failed:\t" << e.message() << '\n';
    }
}

void test_range(const char* host, const char* service)
{
    std::vector<net::address_info> ainfos(10);
    std::error_code e;
    net::getaddrinfo(ainfos.begin(), ainfos.end(), host, service, 0, 0, 0, AI_CANONNAME, e);
    if (e) {
        std::lock_guard<std::mutex> guard(cerr_mutex);
        std::cerr << "test_range failed:\t\t" << e.message() << '\n';
    }
}

void test_reference(const char* host, const char* service)
{
    std::error_code e;
    net::address_info addrinfo;
    net::getaddrinfo(addrinfo, host, service, 0, 0, 0, AI_CANONNAME, e);
    if (e) {
        std::lock_guard<std::mutex> guard(cerr_mutex);
        std::cerr << "test_reference failed:\t\t" << e.message() << '\n';
    }
}

void test_return(const char* host, const char* service)
{
    std::error_code e;
    net::address_info addrinfo = net::getaddrinfo(host, service, 0, 0, 0, AI_CANONNAME, e);
    if (e) {
        std::lock_guard<std::mutex> guard(cerr_mutex);
        std::cerr << "test_return failed:\t\t" << e.message() << '\n';
    }
}

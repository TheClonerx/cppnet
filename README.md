# CPPNET
### An easy to use C++ networking library.

This project aims to create an intuitive and easy to use wrappers around the linux system calls

## Building
This will build just the library.
```bash
cmake -S ./ -B build/ # configure project
cmake --build build/  # build project
```
to build also the examples add `-DCPPNET_BUILD_EXAMPLES=ON` to the configuration arguments

## Running the examples
```bash
./build/examples/example_getaddrinfo google.net 80
```

## Installing
```bash
cmake -S ./ -B build/
cmake --build build/
cmake --install build/
```

## Example

main.cpp
```cpp
#include <iostream>
#include <string_view>
#include <cppnet/socket.hpp>
#include <cppnet/getaddrinfo.hpp>

int main()
{
    // get the address info for google
    net::address_info ainfo = net::getaddrinfo("www.example.com", "80", AF_UNSPEC, SOCK_STREAM, IPPROTO_TCP);
    // create socket for the returned address
    net::socket sock{ ainfo.family(), ainfo.type(), ainfo.protocol() };
    // connect to the given address
    sock.connect(ainfo.address());
    // send a basic HTTP 1.1 request
    sock.send(
        "GET / HTTP/1.1\r\n"
        "Host: www.example.com\r\n"
        "\r\n");
    char buff[2048];
    // receive up to 2048 bytes
    size_t received = sock.recv(buff, sizeof(buff));
    // print out the received bytes
    std::cout << std::string_view{ buff, received } << std::endl;
}
```

### Building
```bash
g++ -std=c++17 -Wall -Wextra -Iinclude/ -Lbuild/ main.cpp -o main -lcppnet
```

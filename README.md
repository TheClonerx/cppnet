# CPPNET
### An easy to use C++ networking library.

This project aims to create an intuitive and easy to use wrappers around the linux system calls

## Building
This will build the library and the examples.
```bash
git clone https://github.com/TheClonerx/cppnet.git
cd cppnet
mkdir build
cmake -S ./ -B build/
cmake --build build/ --parallel $(nproc)
```

## Running the examples
```bash
./bin/examples/example_getaddrinfo google.net 80
```

## Installing
`TODO:` Add install instructions to CMakeLists.txt

## Example

main.cpp
```cpp
#include <iostream>
#include <string_view>
#include <net/socket.hpp>
#include <net/getaddrinfo.hpp>

int main()
{
    // get the address info for google
    net::address_info ainfo = net::getaddrinfo("www.google.net", "80");
    // create socket for the returned address
    net::socket sock{ ainfo.family(), ainfo.type(), ainfo.protocol() };
    // connect to the given address
    sock.connect(ainfo.address());
    // send a basic HTTP 1.1 request
    sock.send("GET / HTTP/1.1\r\n\r\n");
    char buff[2048];
    // receive up to 2048 bytes
    size_t recived = sock.recv(buff, sizeof(buff));
    // print out the received bytes
    std::cout << std::string_view{ buff, recived } << std::endl;
}
```

### Building
```bash
g++ -std=c++17 -Wall -Wextra -Iinclude/ -Lbuild/ main.cpp -o main -lcppnet
```
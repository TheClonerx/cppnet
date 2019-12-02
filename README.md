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
`TODO: ` Add install instructions to CMakeLists.txt

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
    net::addrinfo ainfo = net::getaddrinfo("www.google.net", "80");
    // create of socket for the returned address info
    net::socket sock{ ainfo.family, ainfo.type, ainfo.protocol };
    // connect (implicit conversion from net::addrinfo to net::address)
    sock.connect(ainfo);
    // send a basic HTTP 1.0 request
    sock.send("GET / HTTP/1.0\r\n\r\n");
    char buff[2048];
    // receive up to 2048 bytes
    size_t recived = sock.recv(buff, sizeof(buff));
    // print out the received bytes
    std::cout << std::string_view{ buff, recived } << std::endl;
}
```

### Building
```bash
g++ -Wall -Wextra -Iinclude/ -Lbuild/ -lcppnet main.cpp -o main 
```
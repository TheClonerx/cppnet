# CPPNET
### An easy to use C++ networking library.

This project aims to create an intuitive and easy to use wrappers around the linux system calls

## Building
Building is easy as typing `make` in your terminal! :)

This will build the shared library and the tests.
```bash
git clone https://github.com/TheClonerx/cppnet.git
cd cppnet
make
```

## Running the tests
```bash
export LD_LIBRARY_PATH="$PWD/lib"
./bin/tests/test_getaddrinfo google.net 80
```

## Installing
You can also easily install it in a any directory.

This will copy the headers and the shared library into `MyProject`.
```bash
make install INSTALL_DIR=MyProject
```

## Example

main.cpp
```cpp
#include <iostream>
#include <net/socket.hpp>
#include <string_view>

int main()
{
    net::socket sock;
    sock.connect("google.net", 80);
    sock.send("GET / HTTP/1.0\r\n\r\n");
    char buff[2048];
    size_t recived = sock.recv(buff, sizeof(buff));
    std::cout << std::string_view{ buff, recived } << std::endl;
}
```

building
```bash
g++ -Wall -Wextra -Iinclude/ -Llibs/ -lcppnet main.cpp -o main 
```
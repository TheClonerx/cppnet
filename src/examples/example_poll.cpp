#include "net/poll.hpp"
#include <iostream>

/*
    your terminal maybe has line buffering
*/

using namespace std::literals;

int main()
{
    net::poll poll;
    poll.add(0, POLLIN);
    std::cout << "Now polling stdin...\n";
    size_t polled = poll.execute(5s);
    if (!polled)
        std::cout << "Timeout reached!\n";
    else
        std::cout << "Polled!\n";
}

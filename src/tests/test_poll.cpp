#include "net/poll.hpp"
#include <iostream>

/*
    your terminal maybe has line buffering
*/

int main()
{
    net::poll poll;
    poll.add(0, POLLIN);
    std::cout << "Now polling stdin...\n";
    size_t polled = poll.execute(std::chrono::milliseconds(5000));
    if (!polled)
        std::cout << "Timeout reached!\n";
    else {
        std::cout << "Polled!\n";
    }
}
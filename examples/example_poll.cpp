#include <cppnet/poll.hpp>
#include <iostream>

/*
    your terminal maybe has line buffering
*/

using namespace std::literals;

int main()
{
    net::poll poller;
    poller.add(0, net::poll::events::read);
    std::cout << "Now polling stdin...\n";
    size_t polled = poller.execute(5s);
    if (!polled)
        std::cout << "Timeout reached!\n";
    else
        std::cout << "Polled!\n";
}

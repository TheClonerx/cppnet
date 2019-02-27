#include "net/select.hpp"
#include <iostream>

/*
    your terminal maybe has line buffering
*/

using namespace std::literals;

int main()
{
    net::select selector;
    selector.add_read(0); // adds stdout to read event
    std::cout << "Now selecting stdin...\n";
    auto selected = selector.execute(5s);
    if (!std::get<0>(selected))
        std::cout << "Timeout reached!\n";
    else
        std::cout << "Selected!\n";
}
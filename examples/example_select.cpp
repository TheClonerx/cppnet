#include <cppnet/select.hpp>
#include <iostream>

/*
    your terminal maybe has line buffering
*/

using namespace std::literals;

int main()
{
    net::select selector;
    selector.add(0, net::select::events::read);
    std::cout << "Now selecting stdin...\n";
    auto selected = selector.execute(5s);
    if (!selected.reads)
        std::cout << "Timeout reached!\n";
    else
        std::cout << "Selected!\n";
}

#pragma once
#include <vector>
#include <utility>
#include <chrono>
#include <system_error>
#include <unordered_map>

struct pollfd;

namespace net
{
    class poll
    {
    public:
        void add(int fd, int eventmask);
        void modify(int fd, int eventmask);
        void unregister(int fd);

        std::unordered_map<int, int> get(std::chrono::duration<int, std::milli> timout);
        std::unordered_map<int, int> get(std::chrono::duration<int, std::milli> timout, std::error_code& e); // we still can throw due to out of memory
        
    private:
        std::unordered_map<int, int> fds;
    };
} // net

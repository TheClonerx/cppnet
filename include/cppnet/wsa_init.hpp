#ifndef CPPNET_WSA_INIT_HPP
#define CPPNET_WSA_INIT_HPP

#include <WinSock2.h>

namespace net {
namespace impl {

    class WsaInit {
        static WSADATA wsa_data;
        static bool wsa_inited;

    public:
        WsaInit();
    };
    static WsaInit wsa_init {};

} // namespace impl

} // namespace net

#endif

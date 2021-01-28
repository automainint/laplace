#pragma once

namespace laplace
{
    class socket_library
    {
    public:
        socket_library();
        ~socket_library();

        socket_library(const socket_library &) = delete;
        socket_library(socket_library &&) = delete;

    private:
        bool m_is_ok;
    };

    /*  Equivalent for errno or WSAGetLastError().
     */
    auto socket_error() -> int;

    auto socket_wouldblock() -> int;
    auto socket_msgsize() -> int;
    auto socket_isconn() -> int;
}

#if defined(_WIN32) || defined(_WINDOWS)
#include <winsock2.h>
#include <ws2tcpip.h>

namespace laplace
{
    static constexpr int sockets_version_major = 2;
    static constexpr int sockets_version_minor = 2;
}
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#endif

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

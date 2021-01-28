#include "utils.h"
#include <algorithm>

using namespace laplace;
using namespace std;

auto network::clamp_chunk(size_t size) -> int
{
    return static_cast<int>(min(size, max_chunk_size));
}

auto network::to_string(const sockaddr &a) -> string
{
    char buf[64];

    if (a.sa_family == AF_INET)
    {
        const sockaddr_in &ip4 = reinterpret_cast<const sockaddr_in &>(a);
        ::inet_ntop(AF_INET, &ip4.sin_addr, buf, sizeof buf);
    }
    else
    {
        buf[0] = '\0';
    }

    return buf;
}

auto network::set_mode(SOCKET &s, network::io_mode m) -> bool
{
    if (s == INVALID_SOCKET)
        return true;

    u_long flag = (m == network::sync ? 0 : 1);

    if (::ioctlsocket(s, FIONBIO, &flag) == SOCKET_ERROR)
    {
        ::closesocket(s);
        s = INVALID_SOCKET;

        return false;
    }

    return true;
}

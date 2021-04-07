/*  laplace/platform/socket.h
 *
 *  Copyright (c) 2021 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef laplace_platform_socket_h
#define laplace_platform_socket_h

namespace laplace {
  class socket_library {
  public:
    socket_library(const socket_library &) = delete;
    auto operator=(const socket_library &) -> socket_library & = delete;

    socket_library();
    ~socket_library();

  private:
    bool m_is_ok;
  };

  /*  Equivalent for errno or WSAGetLastError().
   */
  auto socket_error() -> int;

  auto socket_wouldblock() noexcept -> int;
  auto socket_msgsize() noexcept -> int;
  auto socket_isconn() noexcept -> int;
  auto socket_connreset() noexcept -> int;
}

#if !defined(LAPLACE_POSIX_SOCKETS) && \
    (defined(_WIN32) || defined(_WINDOWS))
#  include <winsock2.h>
#  include <ws2tcpip.h>

namespace laplace {
  static constexpr int winsock_version_major = 2;
  static constexpr int winsock_version_minor = 2;
}

using socklen_t = int;
#else
#  include <unistd.h>
#  include <fcntl.h>
#  include <netinet/in.h>
#  include <sys/socket.h>
#  include <sys/types.h>
#  include <sys/ioctl.h>
#  include <arpa/inet.h>

using SOCKET = int;

static constexpr auto INVALID_SOCKET = -1;
static constexpr auto SOCKET_ERROR   = -1;

inline auto closesocket(int s) {
  return ::close(s);
}

/*  TODO
 *  Create mutual functions for Unix/Windows.
 */
inline auto ioctlsocket(int s, int cmd, u_long *arg) -> int {
  if (arg && cmd == FIONBIO) {
    auto flags = ::fcntl(s, F_GETFL, 0);
    if (*arg) flags |= O_NONBLOCK;
    return ::fcntl(s, F_SETFL, flags);
  }

  return -1;
}
#endif

#ifdef min
#  undef min
#endif

#ifdef max
#  undef max
#endif

#endif
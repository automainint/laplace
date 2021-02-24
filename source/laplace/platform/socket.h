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
    socket_library();
    ~socket_library();

    socket_library(const socket_library &) = delete;
    socket_library(socket_library &&)      = delete;

  private:
    bool m_is_ok;
  };

  /*  Equivalent for errno or WSAGetLastError().
   */
  auto socket_error() -> int;

  auto socket_wouldblock() -> int;
  auto socket_msgsize() -> int;
  auto socket_isconn() -> int;
  auto socket_connreset() -> int;
}

#if !defined(LAPLACE_POSIX_SOCKETS) && \
    (defined(_WIN32) || defined(_WINDOWS))
#  include <winsock2.h>
#  include <ws2tcpip.h>

namespace laplace {
  static constexpr int winsock_version_major = 2;
  static constexpr int winsock_version_minor = 2;
}
#else
#  include <netinet/in.h>
#  include <sys/socket.h>
#  include <sys/types.h>
#endif

#ifdef min
#  undef min
#endif

#ifdef max
#  undef max
#endif

#endif
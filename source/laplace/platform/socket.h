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

#if !defined(LAPLACE_POSIX_SOCKETS) && \
    (defined(_WIN32) || defined(_WINDOWS))
#  include <winsock2.h>
#  include <ws2tcpip.h>

#  undef min
#  undef max

namespace laplace {
  static constexpr int winsock_version_major = 2;
  static constexpr int winsock_version_minor = 2;
}

using socket_t  = SOCKET;
using socklen_t = int;
#else
#  include <arpa/inet.h>
#  include <cerrno>
#  include <fcntl.h>
#  include <netinet/in.h>
#  include <sys/ioctl.h>
#  include <sys/select.h>
#  include <sys/socket.h>
#  include <sys/types.h>
#  include <unistd.h>

using socket_t = int;
#endif

namespace laplace {
  class socket_library {
  public:
    socket_library(const socket_library &) = delete;
    auto operator=(const socket_library &) -> socket_library & = delete;

    socket_library() noexcept;
    ~socket_library() noexcept;

  private:
    bool m_is_ok;
  };

  /*  Equivalent for errno or WSAGetLastError().
   */
  auto socket_error() noexcept -> int;

  auto socket_close(socket_t s) noexcept -> int;
  auto socket_set_blocking(socket_t s) noexcept -> int;
  auto socket_set_nonblocking(socket_t s) noexcept -> int;

  auto socket_inprogress() noexcept -> int;
  auto socket_wouldblock() noexcept -> int;
  auto socket_msgsize() noexcept -> int;
  auto socket_isconn() noexcept -> int;
  auto socket_connreset() noexcept -> int;
  auto socket_addrinuse() noexcept -> int;
}

#endif
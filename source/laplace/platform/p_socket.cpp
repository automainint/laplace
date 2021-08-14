/*  laplace/platform/p_socket.cpp
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

#include "socket.h"

#include "../core/defs.h"
#include <iostream>

namespace laplace {
#if !defined(LAPLACE_POSIX_SOCKETS) && \
    (defined(_WIN32) || defined(_WINDOWS))
  /*  Windows implementation.
   */

  socket_library::socket_library() noexcept {
    m_is_ok = true;

    auto version = MAKEWORD(winsock_version_major,
                            winsock_version_minor);

    auto data = WSAData {};

    if (WSAStartup(version, &data) != ERROR_SUCCESS) {
      error_("WSAStartup failed.", __FUNCTION__);
      m_is_ok = false;
    }
  }

  socket_library::~socket_library() noexcept {
    if (m_is_ok) {
      WSACleanup();
      m_is_ok = false;
    }
  }

  auto socket_error() noexcept -> int {
    return WSAGetLastError();
  }

  auto socket_close(socket_t s) noexcept -> int {
    return ::closesocket(s);
  }

  auto socket_set_blocking(socket_t s) noexcept -> int {
    if (s == -1) {
      return -1;
    }

    auto flag = u_long {};

    if (::ioctlsocket(s, FIONBIO, &flag) == -1) {
      return -1;
    }

    return 0;
  }

  auto socket_set_nonblocking(socket_t s) noexcept -> int {
    if (s == -1) {
      return -1;
    }

    auto flag = u_long { 1 };

    if (::ioctlsocket(s, FIONBIO, &flag) == -1) {
      return -1;
    }

    return 0;
  }

  auto socket_inprogress() noexcept -> int {
    return WSAEINPROGRESS;
  }

  auto socket_wouldblock() noexcept -> int {
    return WSAEWOULDBLOCK;
  }

  auto socket_msgsize() noexcept -> int {
    return WSAEMSGSIZE;
  }

  auto socket_isconn() noexcept -> int {
    return WSAEISCONN;
  }

  auto socket_connreset() noexcept -> int {
    return WSAECONNRESET;
  }

  auto socket_addrinuse() noexcept -> int {
    return WSAEADDRINUSE;
  }
#else
  /*  Default platform implementation. Use POSIX sockets.
   */

  socket_library::socket_library() noexcept { }
  socket_library::~socket_library() noexcept { }

  auto socket_error() noexcept -> int {
    return errno;
  }

  auto socket_close(socket_t s) noexcept -> int {
    return ::close(s);
  }

  auto socket_set_blocking(socket_t s) noexcept -> int {
    auto flags = ::fcntl(s, F_GETFL, 0);
    flags &= ~O_NONBLOCK;
    return ::fcntl(s, F_SETFL, flags);
  }

  auto socket_set_nonblocking(socket_t s) noexcept -> int {
    auto flags = ::fcntl(s, F_GETFL, 0);
    flags |= O_NONBLOCK;
    return ::fcntl(s, F_SETFL, flags);
  }

  auto socket_inprogress() noexcept -> int {
    return EINPROGRESS;
  }

  auto socket_wouldblock() noexcept -> int {
    return EWOULDBLOCK;
  }

  auto socket_msgsize() noexcept -> int {
    return EMSGSIZE;
  }

  auto socket_isconn() noexcept -> int {
    return EISCONN;
  }

  auto socket_connreset() noexcept -> int {
    return ECONNRESET;
  }

  auto socket_addrinuse() noexcept -> int {
    return EADDRINUSE;
  }
#endif
}

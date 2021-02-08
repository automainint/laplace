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

#include "../core/defs.h"
#include "socket.h"
#include <iostream>

namespace laplace {
#if !defined(LAPLACE_POSIX_SOCKETS) && \
    (defined(_WIN32) || defined(_WINDOWS))
  /*  Windows implementation.
   */

  socket_library::socket_library() {
    m_is_ok = true;

    WORD version = MAKEWORD(
        winsock_version_major, winsock_version_minor);

    WSAData data;

    if (WSAStartup(version, &data) != ERROR_SUCCESS) {
      error(__FUNCTION__, "WSAStartup failed.");
      m_is_ok = false;
    }
  }

  socket_library::~socket_library() {
    if (m_is_ok) {
      WSACleanup();
      m_is_ok = false;
    }
  }

  auto socket_error() -> int {
    return WSAGetLastError();
  }

  auto socket_wouldblock() -> int {
    return WSAEWOULDBLOCK;
  }

  auto socket_msgsize() -> int {
    return WSAEMSGSIZE;
  }

  auto socket_isconn() -> int {
    return WSAEISCONN;
  }
#else
  /*  Default platform implementation. Use POSIX sockets.
   */

  socket_library::socket_library() { }
  socket_library::~socket_library() { }

  auto socket_error() -> int {
    return errno;
  }

  auto socket_wouldblock() -> int {
    return EWOULDBLOCK;
  }

  auto socket_msgsize() -> int {
    return EMSGSIZE;
  }

  auto socket_isconn() -> int {
    return EISCONN;
  }
#endif
}

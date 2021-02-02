#include "../core/defs.h"
#include "socket.h"
#include <iostream>

using namespace laplace;
using namespace std;

#if !defined(LAPLACE_POSIX_SOCKETS) && \
    (defined(_WIN32) || defined(_WINDOWS))
/*  Windows implementation.
 */

laplace::socket_library::socket_library() {
  m_is_ok = true;

  WORD version = MAKEWORD(laplace::sockets_version_major,
      laplace::sockets_version_minor);

  WSAData data;

  if (WSAStartup(version, &data) != ERROR_SUCCESS) {
    error(__FUNCTION__, "WSAStartup failed.");
    m_is_ok = false;
  }
}

laplace::socket_library::~socket_library() {
  if (m_is_ok) {
    WSACleanup();
    m_is_ok = false;
  }
}

auto laplace::socket_error() -> int {
  return WSAGetLastError();
}

auto laplace::socket_wouldblock() -> int {
  return WSAEWOULDBLOCK;
}

auto laplace::socket_msgsize() -> int {
  return WSAEMSGSIZE;
}

auto laplace::socket_isconn() -> int {
  return WSAEISCONN;
}
#else
/*  Default platform implementation. Use POSIX sockets.
 */

laplace::socket_library::socket_library() { }
laplace::socket_library::~socket_library() { }

auto laplace::socket_error() -> int {
  return errno;
}

auto laplace::socket_wouldblock() -> int {
  return EWOULDBLOCK;
}

auto laplace::socket_msgsize() -> int {
  return EMSGSIZE;
}

auto laplace::socket_isconn() -> int {
  return EISCONN;
}
#endif

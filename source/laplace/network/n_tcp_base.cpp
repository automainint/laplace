/*  laplace/network/n_tcp_base.cpp
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

#include "tcp_base.h"
#include "utils.h"

namespace laplace::network {
  tcp_base::tcp_base() {
    m_is_done = false;
    m_socket  = INVALID_SOCKET;
  }

  tcp_base::~tcp_base() {
    close();
  }

  auto tcp_base::receive_to(uint8_t *p, size_t count, io_mode mode)
      -> size_t {
    size_t size = 0;

    if (m_socket != INVALID_SOCKET &&
        (p != nullptr || count == 0)) {
      auto buf     = reinterpret_cast<char *>(p);
      bool is_sync = false;

      for (; size < count;) {
        auto part = clamp_chunk(count - size);

        auto n = ::recv(m_socket, buf + size, part, 0);

        if (n != SOCKET_ERROR) {
          size += n;
        } else if (socket_error() != socket_wouldblock() ||
                   is_done()) {
          error(__FUNCTION__, "recv failed (code %d).",
                socket_error());
          close();
          break;
        } else if (mode == sync) {
          is_sync = set_mode(m_socket, sync);

          if (!is_sync) {
            error(__FUNCTION__, "ioctlsocket failed (code %d).",
                  socket_error());
            close();
            break;
          }
        } else
          break;
      }

      if (is_sync) {
        if (!set_mode(m_socket, async)) {
          error(__FUNCTION__, "ioctlsocket failed (code %d).",
                socket_error());
          close();
        }
      }
    }

    return size;
  }

  auto tcp_base::receive(size_t count, io_mode mode) -> vbyte {
    vbyte seq;

    if (m_socket != INVALID_SOCKET) {
      seq.resize(count);
      seq.resize(receive_to(seq.data(), count, mode));
    }

    return seq;
  }

  auto tcp_base::send(cref_vbyte seq) -> size_t {
    size_t count = 0;

    if (m_socket != INVALID_SOCKET) {
      auto buf     = reinterpret_cast<const char *>(seq.data());
      bool is_sync = false;

      for (; count < seq.size();) {
        auto part = clamp_chunk(seq.size() - count);

        auto n = ::send(m_socket, buf + count, part, 0);

        if (n != SOCKET_ERROR) {
          count += n;
        } else if (socket_error() != socket_wouldblock() ||
                   is_done()) {
          error(__FUNCTION__, "send failed (code %d).",
                socket_error());
          close();
          break;
        } else {
          is_sync = set_mode(m_socket, sync);

          if (!is_sync) {
            error(__FUNCTION__, "ioctlsocket failed (code %d).",
                  socket_error());
            close();
            break;
          }
        }
      }

      if (is_sync) {
        if (!set_mode(m_socket, async)) {
          error(__FUNCTION__, "ioctlsocket failed (code %d).",
                socket_error());
          close();
        }
      }
    }

    return count;
  }

  auto tcp_base::is_done() const -> bool {
    return m_is_done;
  }

  void tcp_base::close() {
    if (m_socket != INVALID_SOCKET) {
      closesocket(m_socket);
      m_socket = INVALID_SOCKET;
    }
  }

  void tcp_base::done() {
    m_is_done = true;
  }
}

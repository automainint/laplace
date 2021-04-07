/*  laplace/network/n_basic_tcp.cpp
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

#include "../core/string.h"
#include "basic_tcp.h"
#include "utils.h"

namespace laplace::network {
  basic_tcp::basic_tcp() {
    m_is_done = false;
    m_socket  = -1;
  }

  basic_tcp::~basic_tcp() {
    close();
  }

  auto basic_tcp::receive_to(uint8_t *p, size_t count, io_mode mode)
      -> size_t {
    size_t size = 0;

    if (m_socket != -1 && (p != nullptr || count == 0)) {
      auto buf     = reinterpret_cast<char *>(p);
      bool is_sync = false;

      for (; size < count;) {
        auto part = clamp_chunk(count - size);

        auto n = ::recv(m_socket, buf + size, part, 0);

        if (n != -1) {
          size += n;
        } else if (socket_error() != socket_wouldblock() || is_done()) {
          verb(fmt("TCP: recv failed (code: %d).", socket_error()));
          close();
          break;
        } else if (mode == sync) {
          is_sync = set_mode(m_socket, sync);

          if (!is_sync) {
            verb(fmt("TCP: ioctlsocket failed (code: %d).",
                     socket_error()));
            close();
            break;
          }
        } else
          break;
      }

      if (is_sync) {
        if (!set_mode(m_socket, async)) {
          verb(fmt(
              "TCP: ioctlsocket failed (code %d).", socket_error()));
          close();
        }
      }
    }

    return size;
  }

  auto basic_tcp::receive(size_t count, io_mode mode) -> vbyte {
    vbyte seq;

    if (m_socket != -1) {
      seq.resize(count);
      seq.resize(receive_to(seq.data(), count, mode));
    }

    return seq;
  }

  auto basic_tcp::send(span_cbyte seq) -> size_t {
    size_t count = 0;

    if (m_socket != -1) {
      auto buf     = reinterpret_cast<const char *>(seq.data());
      bool is_sync = false;

      for (; count < seq.size();) {
        auto part = clamp_chunk(seq.size() - count);

        auto n = ::send(m_socket, buf + count, part, 0);

        if (n != -1) {
          count += n;
        } else if (socket_error() != socket_wouldblock() || is_done()) {
          verb(fmt("TCP: send failed (code: %d).", socket_error()));
          close();
          break;
        } else {
          is_sync = set_mode(m_socket, sync);

          if (!is_sync) {
            verb(fmt("TCP: ioctlsocket failed (code: %d).",
                     socket_error()));
            close();
            break;
          }
        }
      }

      if (is_sync) {
        if (!set_mode(m_socket, async)) {
          verb(fmt(
              "TCP: ioctlsocket failed (code: %d).", socket_error()));
          close();
        }
      }
    }

    return count;
  }

  auto basic_tcp::is_done() const -> bool {
    return m_is_done;
  }

  void basic_tcp::close() {
    if (m_socket != -1) {
      socket_close(m_socket);
      m_socket = -1;
    }
  }

  void basic_tcp::done() {
    m_is_done = true;
  }
}

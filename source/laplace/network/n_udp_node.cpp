/*  laplace/network/n_udp_node.cpp
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
#include "udp_node.h"
#include "utils.h"

namespace laplace::network {
  using std::string_view, std::string;

  udp_node::udp_node() {
    init();
  }

  udp_node::udp_node(uint16_t port) {
    init();

    bind(port);
  }

  udp_node::~udp_node() {
    if (m_socket != -1) {
      ::closesocket(m_socket);
      m_socket = -1;
    }
  }

  void udp_node::bind(uint16_t port) {
    sockaddr_in name;
    memset(&name, 0, sizeof name);

    name.sin_family      = AF_INET;
    name.sin_port        = ::htons(port);
    name.sin_addr.s_addr = ::htonl(INADDR_ANY);

    auto status = ::bind(m_socket,
                         reinterpret_cast<const ::sockaddr *>(&name),
                         sizeof name);

    if (status == -1) {
      verb(fmt("UDP: bind failed (code %d).", socket_error()));
      return;
    }

    if (port == any_port) {
      ::socklen_t len = sizeof name;

      if (::getsockname(m_socket, reinterpret_cast<::sockaddr *>(&name),
                        &len) == -1) {
        verb(fmt("UDP: getsockname failed (code %d).", socket_error()));
        ::closesocket(m_socket);
        m_socket = -1;
        return;
      }

      m_port = ::ntohs(name.sin_port);
    } else {
      m_port = port;
    }
  }

  auto udp_node::receive_to(uint8_t *p, size_t count, io_mode mode)
      -> size_t {
    size_t size = 0;

    m_is_msgsize   = false;
    m_is_connreset = false;

    if (m_socket != -1 && (p != nullptr || count == 0)) {
      ::socklen_t len = sizeof m_remote;
      memset(&m_remote, 0, sizeof m_remote);

      auto addr    = reinterpret_cast<::sockaddr *>(&m_remote);
      auto buf     = reinterpret_cast<char *>(p);
      bool is_sync = false;

      for (; size < count;) {
        auto part = clamp_chunk(count - size);

        auto n = ::recvfrom(m_socket, buf + size, part, 0, addr, &len);

        if (n != -1) {
          size += n;
        } else if (socket_error() == socket_msgsize()) {
          m_is_msgsize = true;
          break;
        } else if (socket_error() == socket_connreset()) {
          m_is_connreset = true;
          break;
        } else if (socket_error() != socket_wouldblock()) {
          verb(fmt("UDP: recvfrom failed (code %d).", socket_error()));
          break;
        } else if (mode == sync) {
          is_sync = set_mode(m_socket, sync);

          if (!is_sync) {
            verb(fmt("UDP: ioctlsocket failed (code %d).",
                     socket_error()));
            break;
          }
        } else
          break;
      }

      if (is_sync) {
        if (!set_mode(m_socket, async)) {
          verb(fmt(
              "UDP: ioctlsocket failed (code %d).", socket_error()));
        }
      }
    }

    return size;
  }

  auto udp_node::receive(size_t count, io_mode mode) -> vbyte {
    vbyte seq;

    if (m_socket != -1) {
      seq.resize(count);
      seq.resize(receive_to(seq.data(), count, mode));
    }

    return seq;
  }

  auto udp_node::send_to(string_view address, uint16_t port,
                         span_cbyte seq) -> size_t {
    size_t result = 0;

    sockaddr_in name;
    memset(&name, 0, sizeof name);

    name.sin_family = AF_INET;
    name.sin_port   = ::htons(port);

    const auto status = ::inet_pton(
        AF_INET, address.data(), &name.sin_addr.s_addr);

    if (status != 1) {
      verb(fmt("UDP: inet_pton failed (code %d).", socket_error()));
    } else {
      result = send_internal(name, seq);
    }

    return result;
  }

  auto udp_node::get_port() const -> uint16_t {
    return m_port;
  }

  auto udp_node::get_remote_address() const -> string {
    return to_string(reinterpret_cast<const ::sockaddr &>(m_remote));
  }

  auto udp_node::get_remote_port() const -> uint16_t {
    return ::ntohs(m_remote.sin_port);
  }

  auto udp_node::is_msgsize() const noexcept -> bool {
    return m_is_msgsize;
  }

  auto udp_node::is_connreset() const noexcept -> bool {
    return m_is_connreset;
  }

  void udp_node::init() {
    memset(&m_remote, 0, sizeof m_remote);

    m_socket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (m_socket == -1) {
      verb(fmt("UDP: socket failed (code %d).", socket_error()));
      return;
    }

    if (!set_mode(m_socket, async)) {
      verb(fmt("UDP: ioctlsocket failed (code %d).", socket_error()));
    }
  }

  auto udp_node::send_internal(const sockaddr_in &name, span_cbyte seq)
      -> size_t {
    size_t count = 0;

    if (m_socket != -1) {
      auto buf     = reinterpret_cast<const char *>(seq.data());
      auto addr    = reinterpret_cast<const ::sockaddr *>(&name);
      bool is_sync = false;

      for (; count < seq.size();) {
        auto part = clamp_chunk(seq.size() - count);

        auto n = ::sendto(
            m_socket, buf + count, part, 0, addr, sizeof name);

        if (n != -1) {
          if (n <= 0) {
            verb("UDP: 0 bytes sent.");
            break;
          }

          count += n;
        } else if (socket_error() != socket_wouldblock()) {
          verb(fmt("UDP: sendto failed (code %d).", socket_error()));
          break;
        } else {
          verb(fmt("UDP: sendto blocking."));

          is_sync = set_mode(m_socket, sync);

          if (!is_sync) {
            verb(fmt("UDP: ioctlsocket failed (code %d).",
                     socket_error()));
            break;
          }
        }
      }

      if (is_sync) {
        if (!set_mode(m_socket, async)) {
          verb(fmt(
              "UDP: ioctlsocket failed (code %d).", socket_error()));
        }
      }
    }

    return count;
  }
}

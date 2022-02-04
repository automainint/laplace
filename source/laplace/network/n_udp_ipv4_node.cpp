/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "udp_ipv4_node.h"

#include "../core/string.h"
#include "utils.h"

namespace laplace::network {
  using std::string_view, std::string;

  udp_ipv4_node::udp_ipv4_node() noexcept {
    init();
  }

  udp_ipv4_node::udp_ipv4_node(uint16_t port) noexcept {
    init();
    bind(port);
  }

  udp_ipv4_node::~udp_ipv4_node() noexcept {
    if (m_socket == -1)
      return;

    socket_close(m_socket);
    m_socket = -1;
  }

  void udp_ipv4_node::bind(uint16_t port) noexcept {
    auto name = sockaddr_in {};
    memset(&name, 0, sizeof name);

    name.sin_family      = AF_INET;
    name.sin_port        = htons(port);
    name.sin_addr.s_addr = htonl(INADDR_ANY);

    auto status = ::bind(m_socket,
                         reinterpret_cast<const ::sockaddr *>(&name),
                         sizeof name);

    if (status == -1) {
      verb(fmt("UDP: bind failed (code %d).", socket_error()));
      return;
    }

    if (port != any_port) {
      m_port = port;
      return;
    }

    ::socklen_t len = sizeof name;

    if (::getsockname(m_socket, reinterpret_cast<::sockaddr *>(&name),
                      &len) == -1) {
      verb(fmt("UDP: getsockname failed (code %d).", socket_error()));
      socket_close(m_socket);
      m_socket = -1;
      return;
    }

    m_port = ntohs(name.sin_port);
  }

  auto udp_ipv4_node::receive(span_byte data, io_mode mode) noexcept
      -> sl::whole {
    sl::whole received = 0;

    m_is_msgsize   = false;
    m_is_connreset = false;

    if (m_socket != -1 && !data.empty()) {
      ::socklen_t len = sizeof m_remote;
      memset(&m_remote, 0, sizeof m_remote);

      auto addr    = reinterpret_cast<::sockaddr *>(&m_remote);
      auto buf     = reinterpret_cast<char *>(data.data());
      bool is_sync = false;

      if (mode == sync) {
        is_sync = set_mode(m_socket, sync);

        if (!is_sync) {
          verb(fmt("UDP: ioctlsocket failed (code %d).",
                   socket_error()));
          return 0;
        }
      }

      auto n = ::recvfrom(m_socket, buf, clamp_chunk(data.size()), 0,
                          addr, &len);

      if (n != -1) {
        received = n;
      } else if (socket_error() == socket_msgsize()) {
        m_is_msgsize = true;
      } else if (socket_error() == socket_connreset()) {
        m_is_connreset = true;
      } else if (socket_error() != socket_wouldblock()) {
        verb(fmt("UDP: recvfrom failed (code %d).", socket_error()));
      }

      if (is_sync) {
        if (!set_mode(m_socket, async)) {
          verb(fmt("UDP: ioctlsocket failed (code %d).",
                   socket_error()));
        }
      }
    }

    return received;
  }

  auto udp_ipv4_node::send(string_view address, uint16_t port,
                           span_cbyte data) noexcept -> sl::whole {
    sl::whole result = 0;

    auto name = sockaddr_in {};
    memset(&name, 0, sizeof name);

    name.sin_family = AF_INET;
    name.sin_port   = htons(port);

    const auto status = ::inet_pton(AF_INET, address.data(),
                                    &name.sin_addr.s_addr);

    if (status != 1) {
      verb(fmt("UDP: inet_pton failed (code %d).", socket_error()));
    } else {
      result = send_internal(name, data);
    }

    return result;
  }

  auto udp_ipv4_node::get_port() const noexcept -> uint16_t {
    return m_port;
  }

  auto udp_ipv4_node::get_remote_address() const noexcept -> string {
    return to_string(reinterpret_cast<const ::sockaddr &>(m_remote));
  }

  auto udp_ipv4_node::get_remote_port() const noexcept -> uint16_t {
    return ntohs(m_remote.sin_port);
  }

  auto udp_ipv4_node::is_msgsize() const noexcept -> bool {
    return m_is_msgsize;
  }

  auto udp_ipv4_node::is_connreset() const noexcept -> bool {
    return m_is_connreset;
  }

  void udp_ipv4_node::init() noexcept {
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

  auto udp_ipv4_node::send_internal(sockaddr_in const &name,
                                    span_cbyte         data) noexcept
      -> sl::whole {
    sl::whole sent = 0;

    m_is_msgsize   = false;
    m_is_connreset = false;

    if (m_socket != -1) {
      auto buf     = reinterpret_cast<char const *>(data.data());
      auto addr    = reinterpret_cast<::sockaddr const *>(&name);
      bool is_sync = false;

      auto n = ::sendto(m_socket, buf, clamp_chunk(data.size()), 0,
                        addr, sizeof name);

      if (n != -1) {
        if (n <= 0) {
          verb("UDP: 0 bytes sent.");
          return 0;
        }

        sent += n;
      } else if (socket_error() == socket_msgsize()) {
        m_is_msgsize = true;
      } else if (socket_error() == socket_connreset()) {
        m_is_connreset = true;
      } else if (socket_error() != socket_wouldblock()) {
        verb(fmt("UDP: sendto failed (code %d).", socket_error()));
      } else {
        verb(fmt("UDP: sendto blocking."));

        is_sync = set_mode(m_socket, sync);

        if (!is_sync) {
          verb(fmt("UDP: ioctlsocket failed (code %d).",
                   socket_error()));
          return 0;
        }
      }

      if (is_sync && !set_mode(m_socket, async))
        verb(fmt("UDP: ioctlsocket failed (code %d).",
                 socket_error()));
    }

    return sent;
  }
}

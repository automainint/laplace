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
      verb(fmt("UDPv4: bind failed (code %d).", socket_error()));
      return;
    }

    if (port != any_port) {
      m_port = port;
      return;
    }

    ::socklen_t len = sizeof name;

    if (::getsockname(m_socket, reinterpret_cast<::sockaddr *>(&name),
                      &len) == -1) {
      verb(fmt("UDPv4: getsockname failed (code %d).",
               socket_error()));
      socket_close(m_socket);
      m_socket = -1;
      return;
    }

    m_port = ntohs(name.sin_port);
  }

  auto udp_ipv4_node::receive(span_byte data) noexcept -> sl::whole {
    if (m_socket == 0 || data.empty())
      return 0;

    m_is_msgsize   = false;
    m_is_connreset = false;

    ::socklen_t len = sizeof m_remote;
    memset(&m_remote, 0, sizeof m_remote);

    auto addr = reinterpret_cast<::sockaddr *>(&m_remote);
    auto buf  = reinterpret_cast<char *>(data.data());

    auto received = ::recvfrom(
        m_socket, buf, clamp_chunk(data.size()), 0, addr, &len);

    if (received >= 0)
      return received;

    if (received != -1) {
      verb(fmt("UDPv4: Unknown recvfrom return value %d (code %d).",
               received, socket_error()));
      return 0;
    }

    if (socket_error() == socket_msgsize())
      m_is_msgsize = true;
    else if (socket_error() == socket_connreset())
      m_is_connreset = true;
    else if (socket_error() != socket_wouldblock())
      verb(fmt("UDPv4: recvfrom failed (code %d).", socket_error()));

    return 0;
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
      verb(fmt("UDPv4: inet_pton failed (code %d).", socket_error()));
      return 0;
    }

    return send_internal(name, data);
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

  auto udp_ipv4_node::is_size_error() const noexcept -> bool {
    return m_is_msgsize;
  }

  auto udp_ipv4_node::is_connection_reset() const noexcept -> bool {
    return m_is_connreset;
  }

  void udp_ipv4_node::init() noexcept {
    memset(&m_remote, 0, sizeof m_remote);

    m_socket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (m_socket == -1) {
      verb(fmt("UDPv4: socket failed (code %d).", socket_error()));
      return;
    }

    if (socket_set_nonblocking(m_socket) != 0) {
      verb(fmt("UDPv4: fcntl failed (code %d).", socket_error()));
      socket_close(m_socket);
      m_socket = -1;
    }
  }

  auto udp_ipv4_node::send_internal(sockaddr_in const &name,
                                    span_cbyte         data) noexcept
      -> sl::whole {
    if (m_socket == 0 || data.empty())
      return 0;

    m_is_msgsize   = false;
    m_is_connreset = false;

    auto buf  = reinterpret_cast<char const *>(data.data());
    auto addr = reinterpret_cast<::sockaddr const *>(&name);

    auto sent = ::sendto(m_socket, buf, clamp_chunk(data.size()), 0,
                         addr, sizeof name);

    if (sent >= 0)
      return sent;

    if (sent != -1) {
      verb(fmt("UDPv4: Unknown sendto return value %d (code %d).",
               sent, socket_error()));
      return 0;
    }

    if (socket_error() == socket_msgsize())
      m_is_msgsize = true;
    else if (socket_error() == socket_connreset())
      m_is_connreset = true;
    else if (socket_error() == socket_wouldblock())
      verb(fmt("UDPv4: sendto blocking."));
    else
      verb(fmt("UDPv4: sendto failed (code %d).", socket_error()));

    return 0;
  }
}

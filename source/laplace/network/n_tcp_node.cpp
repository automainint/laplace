/*  laplace/network/n_tcp_node.cpp
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
#include "tcp_node.h"
#include "utils.h"

namespace laplace::network {
  using std::string;

  tcp_node::tcp_node(uint16_t port) {
    memset(&m_remote, 0, sizeof m_remote);

    m_listen = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (m_listen == INVALID_SOCKET) {
      done();
      verb(fmt("TCP: socket failed (code %d).", socket_error()));
      return;
    }

    sockaddr_in name;
    memset(&name, 0, sizeof name);

    name.sin_family = AF_INET;
    name.sin_port   = ::htons(port);

    if (::inet_pton(AF_INET, localhost, &name.sin_addr.s_addr) != 1) {
      verb(fmt("TCP: inet_pton failed (code %d).", socket_error()));
      closesocket(m_listen);
      m_listen = INVALID_SOCKET;
      done();
      return;
    }

    if (::bind(m_listen, reinterpret_cast<const sockaddr *>(&name),
               sizeof name) == SOCKET_ERROR) {
      verb(fmt("TCP: bind failed (code %d).", socket_error()));
      ::closesocket(m_listen);
      m_listen = INVALID_SOCKET;
      done();
      return;
    }

    if (port == any_port) {
      int len = sizeof name;

      if (::getsockname(m_listen, reinterpret_cast<sockaddr *>(&name),
                        &len) == SOCKET_ERROR) {
        verb(fmt("TCP: getsockname failed (code %d).", socket_error()));
        ::closesocket(m_listen);
        m_listen = INVALID_SOCKET;
        done();
        return;
      }

      m_port = ::ntohs(name.sin_port);
    } else {
      m_port = port;
    }

    if (::listen(m_listen, SOMAXCONN) == SOCKET_ERROR) {
      verb(fmt("TCP: listen failed (code %d).", socket_error()));
      ::closesocket(m_listen);
      m_listen = INVALID_SOCKET;
      done();
      return;
    }

    if (!set_mode(m_listen, async)) {
      verb(fmt("TCP: ioctlsocket failed (code %d).", socket_error()));
      done();
    }
  }

  tcp_node::~tcp_node() {
    if (m_listen != INVALID_SOCKET) {
      closesocket(m_listen);
      m_listen = INVALID_SOCKET;
      done();
    }
  }

  void tcp_node::on_accept(event_accept fn) {
    m_on_accept = fn;
  }

  void tcp_node::tick() {
    if (!is_done()) {
      if (m_socket != INVALID_SOCKET) {
        if (m_on_accept) {
          m_on_accept(*this);
        }
      } else {
        int len = sizeof m_remote;
        memset(&m_remote, 0, static_cast<size_t>(len));

        m_socket = ::accept(
            m_listen, reinterpret_cast<sockaddr *>(&m_remote), &len);

        if (m_socket != INVALID_SOCKET) {
          if (!set_mode(m_socket, async)) {
            verb(fmt("TCP: ioctlsocket failed (code %d).", socket_error()));
            done();
          } else if (m_on_accept) {
            m_on_accept(*this);
          }
        } else if (socket_error() != socket_wouldblock()) {
          verb(fmt("TCP: accept failed (code %d).", socket_error()));
          done();
        }
      }
    }
  }

  void tcp_node::close_node() {
    done();
  }

  auto tcp_node::is_connected() const -> bool {
    return m_socket != INVALID_SOCKET && !is_done();
  }

  auto tcp_node::get_port() const -> uint16_t {
    return m_port;
  }

  auto tcp_node::get_remote_address() const -> string {
    return to_string(reinterpret_cast<const sockaddr &>(m_remote));
  }

  auto tcp_node::get_remote_port() const -> uint16_t {
    return ::ntohs(m_remote.sin_port);
  }
}

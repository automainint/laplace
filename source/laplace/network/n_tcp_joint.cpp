/*  laplace/network/n_tcp_joint.cpp
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

#include "tcp_joint.h"
#include "utils.h"

namespace laplace::network {
  using std::min, std::copy, std::string_view;

  tcp_joint::tcp_joint(string_view address, uint16_t port) {
    auto size = min(address.size(), sizeof m_address - 1);
    copy(address.begin(), address.begin() + size, m_address);
    m_address[size] = '\0';

    m_port = port;

    m_socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (m_socket == INVALID_SOCKET) {
      verb("TCP: socket failed (code %d).", socket_error());
      done();
      return;
    }

    if (!set_mode(m_socket, async)) {
      verb("TCP: ioctlsocket failed (code %d).", socket_error());
      done();
    }
  }

  tcp_joint::~tcp_joint() { }

  void tcp_joint::on_connect(tcp_joint::event_connect fn) {
    m_on_connect = fn;
  }

  void tcp_joint::tick() {
    if (!is_done()) {
      if (m_socket == INVALID_SOCKET) {
        done();
      } else if (m_is_ready) {
        if (m_on_connect) {
          m_on_connect(*this);
        }
      } else {
        sockaddr_in name;
        memset(&name, 0, sizeof name);

        name.sin_family = AF_INET;
        name.sin_port   = ::htons(m_port);

        if (::inet_pton(AF_INET, m_address, &name.sin_addr.s_addr) !=
            1) {
          verb("TCP: inet_pton failed (code %d).", socket_error());
          done();
        } else if (::connect(m_socket,
                             reinterpret_cast<const sockaddr *>(&name),
                             sizeof name) != SOCKET_ERROR ||
                   socket_error() == socket_isconn()) {
          m_is_ready = true;

          if (m_on_connect) {
            m_on_connect(*this);
          }
        } else if (socket_error() != socket_wouldblock()) {
          verb("TCP: connect failed (code %d).", socket_error());
          done();
        }
      }
    }
  }
}
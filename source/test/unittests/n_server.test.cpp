/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "../../laplace/network/server.h"
#include <gtest/gtest.h>

namespace laplace::test {
  using network::socket_interface, network::io_mode,
      network::any_port, std::copy, std::min, std::max,
      std::unique_ptr, std::make_unique;

  struct mock_socket_interface : public socket_interface {
    struct node_state {
      vbyte    recv;
      vbyte    send;
      uint16_t remote_port = any_port;
      bool     msgsize     = false;
      bool     connreset   = false;
    };

    static sl::vector<node_state> nodes;

    class mock_node : public socket_interface::node {
      node_state *p      = nullptr;
      uint16_t    m_port = 0;

    public:
      mock_node(uint16_t port) {
        if (port == any_port)
          m_port = nodes.size();
        else
          m_port = port;
        nodes.resize(m_port + 1);
        p = nodes.data() + m_port;
      }

      ~mock_node() noexcept override = default;

      auto receive(span_byte buf, io_mode mode) noexcept
          -> sl::whole override {
        auto const size = min(buf.size(), p->recv.size());
        copy(p->recv.begin(), p->recv.begin() + size, buf.begin());
        p->recv.erase(p->recv.begin(), p->recv.begin() + size);
        return size;
      }

      auto send(std::string_view, uint16_t port,
                span_cbyte buf) noexcept -> sl::whole override {
        auto *const q = nodes.data() + port;
        q->recv.resize(q->recv.size() + buf.size());
        q->remote_port = m_port;
        copy(buf.begin(), buf.end(), q->recv.end() - buf.size());
        return buf.size();
      }

      auto get_port() const noexcept -> uint16_t override {
        return m_port;
      }

      auto get_remote_address() const noexcept
          -> std::string override {
        return "localhost";
      }

      auto get_remote_port() const noexcept -> uint16_t override {
        return p->remote_port;
      }

      auto is_msgsize() const noexcept -> bool override {
        return p->msgsize;
      }

      auto is_connreset() const noexcept -> bool override {
        return p->connreset;
      }
    };

    ~mock_socket_interface() noexcept override = default;

    auto open(uint16_t port) noexcept -> unique_ptr<node> override {
      return make_unique<mock_node>(port);
    }
  };

  TEST(network, server) { }
}

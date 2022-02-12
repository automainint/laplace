/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "pipe.h"

#include "../core/string.h"

namespace laplace::network {
  using std::unique_ptr, std::make_unique, std::string, std::min,
      std::copy, std::unique_lock, std::string_view, std::mutex;

  mutex                        pipe::m_sync;
  sl::vector<pipe_node::state> pipe::m_nodes;

  pipe_node::pipe_node(uint16_t port) noexcept {
    m_port = pipe::add(port);
  }

  pipe_node::~pipe_node() noexcept {
    pipe::free(m_port);
  }

  auto pipe_node::receive(span_byte buf) noexcept -> sl::whole {
    return pipe::receive(m_port, buf);
  }

  auto pipe_node::send(string_view address, uint16_t to_port,
                       span_cbyte buf) noexcept -> sl::whole {
    if (!address.empty())
      return 0;
    return pipe::send(m_port, to_port, buf);
  }

  auto pipe_node::get_port() const noexcept -> uint16_t {
    return m_port;
  }

  auto pipe_node::get_remote_address() const noexcept -> string {
    return "";
  }

  auto pipe_node::get_remote_port() const noexcept -> uint16_t {
    return pipe::m_nodes[m_port].remote_port;
  }

  auto pipe_node::is_size_error() const noexcept -> bool {
    return false;
  }

  auto pipe_node::is_connection_reset() const noexcept -> bool {
    return false;
  }

  auto pipe::open(uint16_t port) noexcept -> unique_ptr<node> {
    return make_unique<pipe_node>(port);
  }

  auto pipe::remote_port_of(uint16_t port) noexcept -> uint16_t & {
    if (port <= 0 || port >= m_nodes.size()) {
      error_("Invalid port.", __FUNCTION__);
      static uint16_t invalid = {};
      return invalid;
    }

    return m_nodes[port].remote_port;
  }

  auto pipe::buffer_of(uint16_t port) noexcept -> vbyte & {
    if (port <= 0 || port >= m_nodes.size()) {
      error_("Invalid port.", __FUNCTION__);
      static vbyte invalid = {};
      return invalid;
    }

    return m_nodes[port].buffer;
  }

  auto pipe::add(uint16_t port) noexcept -> uint16_t {
    auto _ul = unique_lock(m_sync);

    if (m_nodes.empty()) {
      m_nodes.resize(1);
      m_nodes[0].remote_port = any_port;
      m_nodes[0].available   = false;
    }

    auto find_node = []() -> uint16_t {
      for (uint16_t n = 1; n < m_nodes.size() && n != 0; n++)
        if (m_nodes[n].available)
          return n;

      m_nodes.resize(m_nodes.size() + 1);
      return m_nodes.size() - 1;
    };

    if (port == any_port) {
      auto const n         = find_node();
      m_nodes[n].available = false;
      return n;
    }

    if (port >= m_nodes.size())
      m_nodes.resize(port + 1);

    if (!m_nodes[port].available)
      return any_port;

    m_nodes[port].available = false;
    return port;
  }

  void pipe::free(uint16_t port) noexcept {
    auto _ul = unique_lock(m_sync);

    if (port > 0 && port < m_nodes.size())
      m_nodes[port].available = true;
  }

  auto pipe::receive(uint16_t port, span_byte buf) noexcept
      -> sl::whole {
    auto _ul = unique_lock(m_sync);

    if (port == any_port || port >= m_nodes.size() ||
        m_nodes[port].available)
      return 0;

    auto      &src  = m_nodes[port].buffer;
    auto const size = min(src.size(), buf.size());
    copy(src.begin(), src.begin() + size, buf.begin());
    src.erase(src.begin(), src.begin() + size);
    return size;
  }

  auto pipe::send(uint16_t from_port, uint16_t to_port,
                  span_cbyte buf) noexcept -> sl::whole {
    auto _ul = unique_lock(m_sync);

    if (from_port == any_port || from_port >= m_nodes.size() ||
        m_nodes[from_port].available || to_port == any_port ||
        to_port >= m_nodes.size() || m_nodes[to_port].available)
      return 0;

    auto &from = m_nodes[from_port];
    auto &to   = m_nodes[to_port];

    if (!to.buffer.empty() && to.remote_port != from_port)
      to.buffer.clear();

    to.remote_port = from_port;
    to.buffer.resize(to.buffer.size() + buf.size());
    copy(buf.begin(), buf.end(), to.buffer.end() - buf.size());
    return buf.size();
  }
}

/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "server.h"

#include "../core/string.h"
#include <iomanip>

namespace laplace::network {
  using std::ostringstream, std::string;

  sl::time const server::default_update_timeout_msec     = 10;
  sl::time const server::default_ping_timeout_msec       = 100;
  sl::time const server::default_connection_timeout_msec = 20000;

  server::~server() {
    m_log.print(fmt("Total bytes sent:      %d", (int) m_total_sent));
    m_log.print(
        fmt("Total bytes received:  %d", (int) m_total_received));

    if (m_total_received > 0) {
      m_log.print(
          fmt("Corruption:  %d bytes (%d%%)", (int) m_total_loss,
              (int) (m_total_loss * 100 + m_total_received / 2) /
                  m_total_received));
    }
  }

  void server::setup_protocol_interface(
      protocol_interface const &in) noexcept {
    if (!check_protocol_interface(in)) {
      error_("Invalid protocol interface.", __FUNCTION__);
      return;
    }

    m_proto = in;
  }

  void server::setup_execution_interface(
      execution_interface const &in) noexcept {
    if (!check_execution_interface(in)) {
      error_("Invalid event interface.", __FUNCTION__);
      return;
    }

    m_exe = in;
  }

  void server::setup_log_interface(
      log_interface const &log) noexcept {
    if (!check_log_interface(log)) {
      error_("Invalid log interface.", __FUNCTION__);
      return;
    }

    m_log = log;
  }

  void server::queue(span_cbyte seq) { }
  void server::tick(sl::time delta_msec) { }
  void server::reconnect() { }

  auto server::get_state() const noexcept -> server_state {
    return m_state;
  }

  auto server::get_bytes_sent() const noexcept -> sl::whole {
    return m_bytes_sent;
  }

  auto server::get_bytes_received() const noexcept -> sl::whole {
    return m_bytes_received;
  }

  auto server::get_bytes_loss() const noexcept -> sl::whole {
    return m_bytes_loss;
  }

  auto server::is_connected() const noexcept -> bool {
    return m_is_connected;
  }

  auto server::is_quit() const noexcept -> bool {
    return m_is_quit;
  }

  void server::set_connected(bool is_connected) noexcept {
    m_is_connected = is_connected;
  }

  void server::set_quit(bool is_quit) noexcept {
    m_is_quit = is_quit;
  }

  void server::set_state(server_state state) noexcept {
    m_state = state;
  }

  void server::reset_tick() noexcept {
    m_total_sent += m_bytes_sent;
    m_total_received += m_bytes_received;
    m_total_loss += m_bytes_loss;

    m_bytes_sent     = 0;
    m_bytes_received = 0;
    m_bytes_loss     = 0;
  }

  void server::add_bytes_sent(sl::whole count) noexcept {
    m_bytes_sent += count;
  }

  void server::add_bytes_received(sl::whole count) noexcept {
    m_bytes_received += count;
  }

  void server::add_bytes_loss(sl::whole count) noexcept {
    m_bytes_loss += count;
  }

  auto server::get_connection_timeout() const noexcept -> sl::time {
    return m_connection_timeout_msec;
  }

  auto server::get_update_timeout() const noexcept -> sl::time {
    return m_update_timeout_msec;
  }

  auto server::get_ping_timeout() const noexcept -> sl::time {
    return m_ping_timeout_msec;
  }
}

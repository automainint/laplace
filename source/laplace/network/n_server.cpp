/*  laplace/network/n_server.cpp
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

#include "../core/utils.h"
#include "server.h"
#include <iomanip>

namespace laplace::network {
  using std::ostringstream, std::hex, std::setw, std::make_shared,
      engine::ptr_factory, engine::ptr_world, engine::ptr_solver,
      engine::solver, engine::world, engine::seed_type;

  server::~server() {
    if (is_verbose()) {
      verb(fmt("Total bytes sent:     %zu", m_total_sent));
      verb(fmt("Total bytes received: %zu", m_total_received));

      if (m_total_received > 0) {
        verb(fmt("Corruption: %zu bytes (%zu%%)", m_total_loss,
                 (m_total_loss * 100 + m_total_received / 2) /
                     m_total_received));
      }
    }
  }

  void server::set_factory(ptr_factory fac) {
    m_factory = fac;
  }

  void server::set_verbose(bool verbose) noexcept {
    m_verbose = verbose;
  }

  void server::queue(span_cbyte seq) { }
  void server::tick(uint64_t delta_msec) { }
  void server::reconnect() { }

  auto server::get_factory() const -> ptr_factory {
    return m_factory;
  }

  auto server::get_solver() const -> ptr_solver {
    return m_solver;
  }

  auto server::get_world() const -> ptr_world {
    return m_world;
  }

  auto server::get_ping() const noexcept -> uint64_t {
    return m_ping_msec;
  }

  auto server::get_state() const noexcept -> server_state {
    return m_state;
  }

  auto server::get_tick_duration() noexcept -> size_t {
    return m_tick_duration_msec;
  }

  auto server::get_bytes_sent() const noexcept -> size_t {
    return m_bytes_sent;
  }

  auto server::get_bytes_received() const noexcept -> size_t {
    return m_bytes_received;
  }

  auto server::get_bytes_loss() const noexcept -> size_t {
    return m_bytes_loss;
  }

  auto server::is_connected() const noexcept -> bool {
    return m_is_connected;
  }

  auto server::is_quit() const noexcept -> bool {
    return m_is_quit;
  }

  auto server::is_verbose() const noexcept -> bool {
    return m_verbose;
  }

  void server::setup_solver() {
    if (!m_solver) {
      m_solver = make_shared<solver>();

      if (m_world) {
        m_solver->set_world(m_world);
      }
    }
  }

  void server::setup_world() {
    if (!m_world) {
      if (!m_solver) {
        m_solver = make_shared<solver>();
      }

      m_world = make_shared<world>();
      m_solver->set_world(m_world);
    }
  }

  void server::set_connected(bool is_connected) noexcept {
    m_is_connected = is_connected;
  }

  void server::set_quit(bool is_quit) noexcept {
    m_is_quit = is_quit;
  }

  void server::set_tick_duration(uint64_t tick_duration_msec) noexcept {
    m_tick_duration_msec = tick_duration_msec;
  }

  void server::set_random_seed(seed_type seed) {
    if (m_solver) {
      m_solver->set_seed(seed);
    }
  }

  void server::set_ping(uint64_t ping_msec) noexcept {
    m_ping_msec = ping_msec;
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

  void server::add_bytes_sent(size_t count) noexcept {
    m_bytes_sent += count;
  }

  void server::add_bytes_received(size_t count) noexcept {
    m_bytes_received += count;
  }

  void server::add_bytes_loss(size_t count) noexcept {
    m_bytes_loss += count;
  }

  auto server::adjust_delta(uint64_t delta_msec) noexcept -> uint64_t {
    uint64_t delta = 0;

    if (m_tick_duration_msec > 0) {
      delta = (m_tick_clock_msec + delta_msec) / m_tick_duration_msec;
      m_tick_clock_msec = delta_msec % m_tick_duration_msec;
    }

    return delta;
  }

  auto server::get_connection_timeout() const noexcept -> uint64_t {
    return m_connection_timeout_msec;
  }

  auto server::get_update_timeout() const noexcept -> uint64_t {
    return m_update_timeout_msec;
  }

  auto server::get_ping_timeout() const noexcept -> uint64_t {
    return m_ping_timeout_msec;
  }

  auto server::get_overtake_factor() const noexcept -> size_t {
    return m_overtake_factor;
  }

  void server::dump(span_cbyte bytes) {
    if (m_verbose) {
      auto ss = ostringstream {};
      ss << " ";
      for (size_t i = 0; i < bytes.size(); i++) {
        ss << " " << setw(2) << hex << static_cast<unsigned>(bytes[i]);
        if ((i % 16) == 15)
          ss << "\n ";
      }
      verb(fmt("\n  DUMP\n%s\n", ss.str().c_str()));
    }
  }
}

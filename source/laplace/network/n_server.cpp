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

  void server::set_factory(ptr_factory fac) {
    m_factory = fac;
  }

  void server::set_verbose(bool verbose) {
    m_verbose = verbose;
  }

  void server::queue(cref_vbyte seq) { }
  void server::tick(size_t delta_msec) { }

  auto server::get_factory() const -> ptr_factory {
    return m_factory;
  }

  auto server::get_solver() const -> ptr_solver {
    return m_solver;
  }

  auto server::get_world() const -> ptr_world {
    return m_world;
  }

  auto server::get_ping() const -> uint64_t {
    return m_ping_msec;
  }

  auto server::get_state() const -> server_state {
    return m_state;
  }

  auto server::get_tick_duration() -> size_t {
    return m_tick_duration_msec;
  }

  auto server::get_bytes_sent() const -> size_t {
    return m_bytes_sent;
  }

  auto server::get_bytes_received() const -> size_t {
    return m_bytes_received;
  }

  auto server::is_verbose() const -> bool {

    return m_verbose;
  }

  auto server::is_connected() const -> bool {
    return m_is_connected;
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

  void server::set_connected(bool is_connected) {
    m_is_connected = is_connected;
  }

  void server::set_tick_duration(uint64_t tick_duration_msec) {
    m_tick_duration_msec = tick_duration_msec;
  }

  void server::set_random_seed(seed_type seed) {
    if (m_solver) {
      m_solver->set_seed(seed);
    }
  }

  void server::set_ping(uint64_t ping_msec) {
    m_ping_msec = ping_msec;
  }

  void server::set_state(server_state state) {
    m_state = state;
  }

  void server::reset_tick() {
    m_bytes_sent     = 0;
    m_bytes_received = 0;
  }

  void server::add_bytes_sent(size_t count) {
    m_bytes_sent += count;
  }

  void server::add_bytes_received(size_t count) {
    m_bytes_received += count;
  }

  auto server::adjust_delta(size_t delta_msec) -> uint64_t {
    uint64_t delta = 0;

    if (m_tick_duration_msec > 0) {
      delta = (m_tick_clock_msec + delta_msec) / m_tick_duration_msec;
      m_tick_clock_msec = delta_msec % m_tick_duration_msec;
    }

    return delta;
  }

  auto server::get_connection_timeout() const -> uint64_t {
    return m_connection_timeout_msec;
  }

  auto server::get_update_timeout() const -> uint64_t {
    return m_update_timeout_msec;
  }

  auto server::get_ping_timeout() const -> uint64_t {
    return m_ping_timeout_msec;
  }

  void server::dump(cref_vbyte bytes) {
    if (m_verbose) [[unlikely]] {
      ostringstream ss;
      ss << " ";
      for (size_t i = 0; i < bytes.size(); i++) {
        ss << " " << setw(2) << hex << static_cast<unsigned>(bytes[i]);
        if ((i % 16) == 15)
          ss << "\n ";
      }
      verb("  DUMP\n%s\n", ss.str().c_str());
    }
  }
}
/*  laplace/engine/e_server.cpp
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

namespace laplace::engine {
  using std::ostringstream, std::hex, std::setw, std::make_shared;

  void server::set_factory(ptr_factory fac) {
    m_factory = fac;
  }

  void server::set_verbose(bool verbose) {
    m_verbose = verbose;
  }

  void server::queue(cref_vbyte seq) {
    if (m_factory && m_solver) {
      m_solver->apply(m_factory->decode(seq));
    }
  }

  void server::tick(size_t delta_msec) {
    if (m_state == server_state::action && m_solver) {
      m_solver->schedule(adjust_delta(delta_msec));
    }
  }

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

  auto server::adjust_delta(size_t delta_msec) -> uint64_t {
    uint64_t delta = 0;

    if (m_tick_duration_msec > 0) {
      delta = (m_tick_clock_msec + delta_msec) /
              m_tick_duration_msec;
      m_tick_clock_msec = delta_msec % m_tick_duration_msec;
    }

    return delta;
  }

  void server::dump(cref_vbyte bytes) {
    if (m_verbose) [[unlikely]] {
      ostringstream ss;
      ss << " ";
      for (size_t i = 0; i < bytes.size(); i++) {
        ss << " " << setw(2) << hex
           << static_cast<unsigned>(bytes[i]);
        if ((i % 16) == 15)
          ss << "\n ";
      }
      verb("  DUMP\n%s\n", ss.str().c_str());
    }
  }
}

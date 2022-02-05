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

#include "../engine/world.h"
#include <iomanip>

namespace laplace::network {
  using std::ostringstream, std::hex, std::setw, std::make_shared,
      std::move, engine::ptr_factory, engine::ptr_world,
      engine::ptr_solver, engine::solver, engine::world,
      engine::seed_type, engine::basic_factory, engine::prime_impact,
      engine::ptr_impact;

  bool const      server::default_verbose                 = false;
  sl::time const  server::default_tick_duration_msec      = 10;
  sl::time const  server::default_update_timeout_msec     = 10;
  sl::time const  server::default_ping_timeout_msec       = 100;
  sl::time const  server::default_connection_timeout_msec = 20000;
  sl::whole const server::default_overtake_factor         = 3;

  server::~server() {
    if (!is_verbose())
      return;

    verb(fmt("Total bytes sent:      %zu", m_total_sent));
    verb(fmt("Total bytes received:  %zu", m_total_received));

    if (m_total_received > 0) {
      verb(fmt("Corruption:  %zu bytes (%zu%%)", m_total_loss,
               (m_total_loss * 100 + m_total_received / 2) /
                   m_total_received));
    }
  }

  void server::set_factory(ptr_factory factory) noexcept {
    m_factory = factory;
    setup_callbacks();
  }

  void server::set_verbose(bool verbose) noexcept {
    m_verbose = verbose;
  }

  void server::queue(span_cbyte seq) { }
  void server::tick(sl::time delta_msec) { }
  void server::reconnect() { }

  auto server::get_factory() const noexcept -> ptr_factory {
    return m_factory;
  }

  auto server::get_solver() const noexcept -> ptr_solver {
    return m_solver;
  }

  auto server::get_world() const noexcept -> ptr_world {
    return m_world;
  }

  auto server::get_ping() const noexcept -> sl::time {
    return m_ping_msec;
  }

  auto server::get_state() const noexcept -> server_state {
    return m_state;
  }

  auto server::get_tick_duration() const noexcept -> sl::whole {
    return m_tick_duration_msec;
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

  auto server::is_verbose() const noexcept -> bool {
    return m_verbose;
  }

  void server::setup_callbacks() noexcept {
    if (!m_solver)
      return;

    if (m_world) {
      m_solver->on_reset([wor = m_world]() { wor->clear(); });
      m_solver->on_seed([wor = m_world](seed_type seed_value) {
        wor->get_random().seed(seed_value);
      });
      m_solver->on_tick(
          [wor = m_world](sl::time delta) { wor->tick(delta); });
      m_solver->on_schedule(
          [wor = m_world](sl::time delta) { wor->schedule(delta); });
      m_solver->on_join([wor = m_world]() { wor->join(); });
      m_solver->on_queue([wor = m_world](ptr_impact event) {
        wor->queue(move(event));
      });
    }

    if (m_factory)
      m_solver->on_decode([fac = m_factory](span_cbyte event) {
        return fac->decode(event);
      });
  }

  void server::setup_solver() noexcept {
    if (m_solver)
      return;

    m_solver = make_shared<solver>();
    setup_callbacks();
  }

  void server::setup_world() noexcept {
    if (m_world)
      return;

    if (!m_solver)
      m_solver = make_shared<solver>();

    m_world = make_shared<world>();
    setup_callbacks();
  }

  void server::set_connected(bool is_connected) noexcept {
    m_is_connected = is_connected;
  }

  void server::set_quit(bool is_quit) noexcept {
    m_is_quit = is_quit;
  }

  void server::set_tick_duration(
      sl::time tick_duration_msec) noexcept {
    m_tick_duration_msec = tick_duration_msec;
  }

  void server::set_random_seed(seed_type seed) {
    if (m_solver)
      m_solver->set_seed(seed);
  }

  void server::set_ping(sl::time ping_msec) noexcept {
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

  void server::add_bytes_sent(sl::whole count) noexcept {
    m_bytes_sent += count;
  }

  void server::add_bytes_received(sl::whole count) noexcept {
    m_bytes_received += count;
  }

  void server::add_bytes_loss(sl::whole count) noexcept {
    m_bytes_loss += count;
  }

  auto server::adjust_delta(sl::time delta_msec) noexcept
      -> sl::time {
    auto delta = sl::time {};

    if (m_tick_duration_msec > 0) {
      delta = (m_tick_clock_msec + delta_msec) / m_tick_duration_msec;
      m_tick_clock_msec = (m_tick_clock_msec + delta_msec) %
                          m_tick_duration_msec;
    }

    return delta;
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

  auto server::get_overtake_factor() const noexcept -> sl::whole {
    return m_overtake_factor;
  }

  void server::verb_queue(sl::index  n,
                          span_cbyte seq) const noexcept {
    if (!is_verbose())
      return;

    auto const id   = prime_impact::get_id(seq);
    auto const name = basic_factory::name_by_id_native(id);

    if (!name.empty())
      verb(fmt(" :: queue %4d '%s (%d)'", (int) n, name.c_str(),
               (int) id));
    else
      verb(fmt(" :: queue %4d '%d'", (int) n, (int) id));
  }

  void server::verb_slot(sl::index slot, sl::index n,
                         span_cbyte seq) const noexcept {
    if (!is_verbose())
      return;

    auto const id   = prime_impact::get_id(seq);
    auto const name = basic_factory::name_by_id_native(id);

    if (!name.empty())
      verb(fmt(" :: (slot %2d) %4d '%s (%d)'", (int) slot, (int) n,
               name.c_str(), (int) id));
    else
      verb(fmt(" :: (slot %2d) %4d '%d'", (int) slot, (int) n,
               (int) id));
  }

  void server::dump(span_cbyte bytes) const noexcept {
    if (!is_verbose())
      return;

    auto ss = ostringstream {};
    ss << " ";
    for (sl::index i = 0; i < bytes.size(); i++) {
      ss << " " << setw(2) << hex << static_cast<unsigned>(bytes[i]);
      if ((i % 16) == 15)
        ss << "\n ";
    }
    verb(fmt("\n  DUMP\n%s\n", ss.str().c_str()));
  }
}

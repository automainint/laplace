/*  Copyright (c) 2021 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "solver.h"

#include "prime_impact.h"
#include <algorithm>

namespace laplace::engine {
  const bool solver::default_allow_rewind = false;

  void solver::set_factory(ptr_factory f) {
    m_factory = f;
  }

  void solver::set_world(ptr_world w) {
    if (m_world != w) {
      m_world = w;

      if (m_world) {
        m_world->get_random().seed(m_seed);
      }
    }
  }

  void solver::reset_factory() {
    m_factory.reset();
  }

  void solver::reset_world() {
    m_world.reset();
  }

  auto solver::apply(span_cbyte ev) -> sl::time {
    if (ev.empty()) {
      error_("No event.", __FUNCTION__);
      return time_undefined;
    }

    auto event_time = prime_impact::get_time(ev);

    if (event_time == time_undefined) {
      event_time = m_time;
    }

    if (event_time < m_time) {
      if (!m_is_rewind_allowed) {
        error_("Rewind is not allowed. Event ignored.", __FUNCTION__);
        return time_undefined;
      }

      auto const current_time = m_time;
      rewind_to(event_time);
      m_history.emplace(m_history.begin() + m_position, ev.begin(),
                        ev.end());
      rewind_to(current_time);

    } else {
      auto const it = lower_bound(
          m_history.begin() + m_position, m_history.end(), event_time,
          [](vbyte const &a, sl::time b) -> bool {
            return prime_impact::get_time(a) < b;
          });

      auto const n = it - m_history.begin();

      m_history.emplace(it, ev.begin(), ev.end());

      prime_impact::set_time(m_history[n], event_time);
    }

    return event_time;
  }

  void solver::rewind_to(sl::time time) {
    adjust(time);
    join();
  }

  void solver::schedule(sl::time delta) {
    adjust(m_time + delta);
  }

  void solver::join() {
    if (m_world) {
      m_world->join();
    }
  }

  void solver::allow_rewind(bool is_rewind_allowed) {
    m_is_rewind_allowed = is_rewind_allowed;
  }

  auto solver::is_rewind_allowed() const -> bool {
    return m_is_rewind_allowed;
  }

  auto solver::get_time() const -> sl::time {
    return m_time;
  }

  auto solver::get_position() const -> sl::index {
    return m_position;
  }

  void solver::set_seed(seed_type seed) {
    m_seed = seed;

    if (m_world) {
      m_world->get_random().seed(m_seed);
    }
  }

  auto solver::get_seed() const -> seed_type {
    return m_seed;
  }

  void solver::clear_history() {
    m_time     = 0;
    m_position = 0;

    m_history.clear();
  }

  auto solver::get_history_count() const -> sl::whole {
    return m_history.size();
  }

  auto solver::get_history(sl::index n) const -> vbyte {
    if (n < 0 || n >= m_history.size()) {
      error_("Invalid index.", __FUNCTION__);
      return {};
    }

    return m_history[n];
  }

  auto solver::generate_seed() -> seed_type {
    auto dev  = std::random_device {};
    auto dist = std::uniform_int_distribution<seed_type> {};

    return dist(dev);
  }

  void solver::adjust(sl::time time) {
    if (m_time == time) {
      return;
    }

    if (time < m_time) {
      if (m_world) {
        m_world->clear();
        m_world->get_random().seed(m_seed);
      }

      m_time     = 0;
      m_position = 0;
    }

    auto const i_end = lower_bound(
        m_history.begin() + m_position, m_history.end(), time,
        [](vbyte const &a, sl::time b) -> bool {
          return prime_impact::get_time(a) < b;
        });

    auto t0 = m_time;

    for (auto i = m_history.begin() + m_position; i != i_end; i++) {
      auto t1 = prime_impact::get_time(*i);

      if (t0 < t1) {
        if (m_world) {
          m_world->tick(t1 - t0);
        }
        t0 = t1;
      }

      if (m_world && m_factory) {
        m_world->queue(m_factory->decode(*i));
      }
    }

    if (t0 < time && m_world) {
      m_world->schedule(time - t0);
    }

    m_time     = time;
    m_position = static_cast<size_t>(i_end - m_history.begin());
  }
}

/*  Copyright (c) 2022 Mitya Selivanov
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
#include <utility>

namespace laplace::engine {
  using std::move;

  void solver::on_reset(fn_reset callback) noexcept {
    m_reset = move(callback);
  }

  void solver::on_seed(fn_seed callback) noexcept {
    m_set_seed = move(callback);
  }

  void solver::on_tick(fn_tick callback) noexcept {
    m_tick = move(callback);
  }

  void solver::on_schedule(fn_schedule callback) noexcept {
    m_schedule = move(callback);
  }

  void solver::on_join(fn_join callback) noexcept {
    m_join = move(callback);
  }

  void solver::on_queue(fn_queue callback) noexcept {
    m_queue = move(callback);
  }

  void solver::on_decode(fn_decode callback) noexcept {
    m_decode = move(callback);
  }

  auto solver::apply(span_cbyte event) noexcept -> sl::time {
    if (event.empty()) {
      log(log_event::error, "No event.", __FUNCTION__);
      return time_undefined;
    }

    auto event_time = prime_impact::get_time(event);

    if (event_time == time_undefined)
      event_time = m_time;

    if (event_time < m_time) {
      log(log_event::error, "Invalid time. Event ignored.",
          __FUNCTION__);
      return time_undefined;
    }

    m_history.emplace_back(event.begin(), event.end());
    prime_impact::set_time(m_history.back(), event_time);

    return event_time;
  }

  void solver::rewind_to(sl::time time) noexcept {
    if (m_time == time)
      return;

    if (time < m_time) {
      if (m_reset)
        m_reset();

      m_time     = 0;
      m_position = 0;
    }

    if (time <= 0)
      return;

    if (m_time == 0 && m_position == 0 && m_set_seed)
      m_set_seed(m_seed);

    auto const i_end = lower_bound(
        m_history.begin() + m_position, m_history.end(), time,
        [](vbyte const &event, sl::time time_point) -> bool {
          return prime_impact::get_time(event) < time_point;
        });

    auto time_previous = m_time;
    bool joined        = false;

    for (auto i = m_history.begin() + m_position; i != i_end; i++) {
      auto time_now = prime_impact::get_time(*i);

      if (time_previous < time_now) {
        if (m_tick)
          m_tick(time_now - time_previous);

        time_previous = time_now;
        joined        = true;
      }

      if (m_decode && m_queue) {
        if (!joined) {
          joined = true;
          if (m_join)
            m_join();
        }

        m_queue(m_decode(*i));
      }
    }

    if (time_previous < time && m_schedule)
      m_schedule(time - time_previous);

    m_time     = time;
    m_position = i_end - m_history.begin();
  }

  void solver::schedule(sl::time delta) noexcept {
    if (delta <= 0) {
      if (delta < 0)
        log(log_event::error, "Invalid time.", __FUNCTION__);
      return;
    }

    rewind_to(m_time + delta);
  }

  auto solver::get_time() const noexcept -> sl::time {
    return m_time;
  }

  auto solver::get_position() const noexcept -> sl::index {
    return m_position;
  }

  void solver::set_seed(seed_type seed) noexcept {
    m_seed = seed;

    if (m_set_seed)
      m_set_seed(m_seed);
  }

  auto solver::get_seed() const noexcept -> seed_type {
    return m_seed;
  }

  void solver::clear_history() noexcept {
    m_time     = 0;
    m_position = 0;

    m_history.clear();
  }

  auto solver::get_history() const noexcept
      -> std::span<vbyte const> {
    return m_history;
  }

  auto solver::generate_seed() noexcept -> seed_type {
    auto dev  = std::random_device {};
    auto dist = std::uniform_int_distribution<seed_type> {};

    return dist(dev);
  }
}

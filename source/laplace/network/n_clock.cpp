/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "clock.h"

namespace laplace::network {
  sl::time const  clock::default_tick_duration   = 10;
  sl::time const  clock::default_latency         = 10;
  sl::whole const clock::default_overtake_factor = 3;

  void clock::set_tick_duration(sl::time tick_duration) noexcept {
    if (tick_duration <= 0) {
      log(log_event::error, "Invalid tick duration.", __FUNCTION__);
      return;
    }

    m_tick_duration = tick_duration;
  }

  void clock::set_latency(sl::time latency) noexcept {
    if (latency < 0) {
      log(log_event::error, "Invalid latency.", __FUNCTION__);
      return;
    }

    m_latency = latency;
  }

  void clock::set_overtake_factor(
      sl::whole overtake_factor) noexcept {
    if (overtake_factor <= 0) {
      log(log_event::error, "Invalid overtake factor.", __FUNCTION__);
      return;
    }

    m_overtake_factor = overtake_factor;
  }

  void clock::set_master_time(sl::time master_time) noexcept {
    if (master_time > m_master_time)
      m_master_time = master_time;
  }

  void clock::tick(sl::time time_elapsed) noexcept {
    m_time_elapsed = eval_tick_count(time_elapsed);
    add(m_local_time, time_elapsed);
    add(m_time, m_time_elapsed);
  }

  auto clock::is_overflow() const noexcept -> bool {
    return m_is_overflow;
  }

  auto clock::get_tick_duration() const noexcept -> sl::time {
    return m_tick_duration;
  }

  auto clock::get_latency() const noexcept -> sl::time {
    return m_latency;
  }

  auto clock::get_overtake_factor() const noexcept -> sl::whole {
    return m_overtake_factor;
  }

  auto clock::get_local_time() const noexcept -> sl::time {
    return m_local_time;
  }

  auto clock::get_time() const noexcept -> sl::time {
    return m_time;
  }

  auto clock::get_time_elapsed() const noexcept -> sl::time {
    return m_time_elapsed;
  }

  void clock::add(sl::time &left, sl::time right) noexcept {
    auto const sum = static_cast<sl::time>(
        static_cast<uint64_t>(left) + static_cast<uint64_t>(right));
    if (sum < left)
      m_is_overflow = true;
    left = sum;
  }

  auto clock::eval_tick_count(sl::time time_elapsed) noexcept
      -> sl::time {
    auto const delta = adjust_delta(time_elapsed);
    if (m_master_time < 0)
      return delta;
    return std::min(delta + adjust_overtake(m_time),
                    m_master_time - m_time);
  }

  auto clock::adjust_delta(sl::time time_elapsed) noexcept
      -> sl::time {
    auto const delta = (m_tick_remainder + time_elapsed) /
                       m_tick_duration;
    m_tick_remainder = (m_tick_remainder + time_elapsed) %
                       m_tick_duration;
    return delta;
  }

  auto clock::adjust_overtake(sl::time time) const noexcept
      -> sl::time {
    auto latency_ticks = m_latency /
                         (m_tick_duration * m_overtake_factor);
    auto time_gape = m_master_time - time;
    return std::max<sl::time>(0, time_gape - latency_ticks);
  }
}

/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef laplace_network_clock_h
#define laplace_network_clock_h

#include "../core/defs.h"

namespace laplace::network {
  class clock {
  public:
    static constexpr sl::time const no_master = -1;

    static sl::time const  default_tick_duration;
    static sl::time const  default_latency;
    static sl::whole const default_overtake_factor;

    void set_tick_duration(sl::time tick_duration) noexcept;
    void set_latency(sl::time latency) noexcept;
    void set_overtake_factor(sl::whole overtake_factor) noexcept;
    void set_master_time(sl::time master_time) noexcept;

    void tick(sl::time time_elapsed) noexcept;

    [[nodiscard]] auto is_overflow() const noexcept -> bool;
    [[nodiscard]] auto get_tick_duration() const noexcept -> sl::time;
    [[nodiscard]] auto get_latency() const noexcept -> sl::time;
    [[nodiscard]] auto get_overtake_factor() const noexcept
        -> sl::whole;
    [[nodiscard]] auto get_local_time() const noexcept -> sl::time;
    [[nodiscard]] auto get_time() const noexcept -> sl::time;
    [[nodiscard]] auto get_time_elapsed() const noexcept -> sl::time;

  private:
    void add(sl::time &left, sl::time right) noexcept;

    [[nodiscard]] auto eval_tick_count(sl::time time_elapsed) noexcept
        -> sl::time;

    [[nodiscard]] auto adjust_delta(sl::time time_elapsed) noexcept
        -> sl::time;

    [[nodiscard]] auto adjust_overtake(sl::time time) const noexcept
        -> sl::time;

    bool m_is_overflow = false;

    sl::time m_master_time  = no_master;
    sl::time m_local_time   = 0;
    sl::time m_time         = 0;
    sl::time m_time_elapsed = 0;

    sl::time  m_tick_duration   = default_tick_duration;
    sl::time  m_latency         = default_latency;
    sl::whole m_overtake_factor = default_overtake_factor;
    sl::time  m_tick_remainder  = 0;
  };
}

#endif

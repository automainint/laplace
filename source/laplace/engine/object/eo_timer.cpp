/*  laplace/engine/object/eo_timer.cpp
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

#include "timer.h"

#include <utility>

namespace laplace::engine::object {
  timer timer::m_proto(timer::proto);

  timer::timer(proto_tag) noexcept : basic_entity(dynamic) { }

  timer::timer(impact_gen gen, intval period,
               sl::whole count) noexcept :
      m_count(count),
      m_gen(std::move(gen)) {
    *this = m_proto;
    init(n_tick_period, period);
    reset_clock();
  }

  void timer::tick(access::world w) noexcept {
    if (m_gen)
      w.queue(m_gen());

    if (m_count > 0 && --m_count == 0)
      self_destruct(w);
  }
}

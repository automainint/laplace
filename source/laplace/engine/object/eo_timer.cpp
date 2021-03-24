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

#include "../action/remove.h"
#include "timer.h"

namespace laplace::engine::object {
  timer timer::m_proto(timer::proto);

  timer::timer(proto_tag) :
      basic_entity(true, false, false, false) { }

  timer::timer(impact_gen gen, uint64_t period, size_t count) {
    *this = m_proto;

    init(n_tick_period, static_cast<int64_t>(period));

    reset_clock();

    m_count = count;
    m_gen   = gen;
  }

  void timer::tick(access::world w) {
    if (m_gen) {
      w.queue(m_gen());
    }

    if (m_count > 0 && --m_count == 0) {
      self_destruct(w);
    }
  }
}

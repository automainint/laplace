/*  laplace/engine/object/timer.h
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

#ifndef laplace_engine_object_timer_h
#define laplace_engine_object_timer_h

#include "../basic_entity.h"
#include "../basic_impact.h"

namespace laplace::engine::object {
  /*  Timer entity. Dynamic, self-destructive.
   */
  class timer final : public basic_entity {
  public:
    timer(impact_gen gen, uint64_t period, size_t count = 1);
    ~timer() final = default;

    void tick(access::world w) final;

  private:
    timer(proto_tag);

    static timer m_proto;

    size_t     m_count = 1;
    impact_gen m_gen;
  };
}

#endif

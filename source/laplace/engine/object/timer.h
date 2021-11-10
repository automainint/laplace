/*  Copyright (c) 2021 Mitya Selivanov
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
    timer(timer const &) noexcept = default;
    timer(timer &&) noexcept      = default;
    auto operator=(timer const &) noexcept -> timer & = default;
    auto operator=(timer &&) noexcept -> timer & = default;

    timer(impact_gen gen, intval period,
          sl::whole count = 1) noexcept;

    ~timer() noexcept final = default;

    void tick(access::world w) noexcept final;

  private:
    explicit timer(proto_tag) noexcept;

    static timer m_proto;

    sl::whole  m_count = 1;
    impact_gen m_gen;
  };
}

#endif

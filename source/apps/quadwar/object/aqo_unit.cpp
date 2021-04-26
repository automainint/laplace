/*  apps/quadwar/object/aqo_unit.cpp
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

#include "../action/unit_move.h"
#include "unit.h"

namespace quadwar_app::object {
  namespace access = engine::access;

  using engine::intval, engine::vec2i, std::make_shared,
      action::unit_move;

  size_t unit::n_actor  = {};
  size_t unit::n_x      = {};
  size_t unit::n_y      = {};
  size_t unit::n_health = {};

  unit unit::m_proto(unit::proto);

  unit::unit(proto_tag) : basic_entity(1) {
    setup_sets(
        { { .id = sets::unit_actor, .scale = 1 },
          { .id = sets::unit_x, .scale = sets::scale_real },
          { .id = sets::unit_y, .scale = sets::scale_real },
          { .id = sets::unit_health, .scale = sets::scale_points } });

    n_actor  = index_of(sets::unit_actor);
    n_x      = index_of(sets::unit_x);
    n_y      = index_of(sets::unit_y);
    n_health = index_of(sets::unit_health);
  }

  unit::unit() : basic_entity(dummy) {
    *this = m_proto;
  }

  void unit::tick(access::world w) {
    w.queue(make_shared<unit_move>(get_id()));
  }

  void unit::set_x(entity en, intval x) {
    en.set(n_x, x);
  }

  void unit::set_y(entity en, intval y) {
    en.set(n_y, y);
  }

  void unit::set_position(entity en, vec2i p) {
    set_x(en, p.x());
    set_y(en, p.y());
  }

  void unit::set_health(entity en, intval health) {
    en.set(n_health, health);
  }

  auto unit::get_x(entity en) -> intval {
    return en.get(n_x);
  }

  auto unit::get_y(entity en) -> intval {
    return en.get(n_y);
  }

  auto unit::get_position(entity en) -> vec2i {
    return vec2i { get_x(en), get_y(en) };
  }

  auto unit::get_health(entity en) -> intval {
    return en.get(n_health);
  }
}

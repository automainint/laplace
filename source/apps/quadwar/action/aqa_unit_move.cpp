/*  apps/quadwar/action/aqa_unit_move.cpp
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

#include "unit_move.h"
#include "../object/unit.h"

namespace quadwar_app::action {
  namespace access = engine::access;

  using object::unit;

  unit_move::unit_move(size_t id_unit) {
    m_id_unit = id_unit;
  }

  void unit_move::perform(access::world w) const {
    auto u = w.get_entity(m_id_unit);

  }
}

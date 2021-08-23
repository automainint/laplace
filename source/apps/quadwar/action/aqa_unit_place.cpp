/*  apps/quadwar/action/aqa_unit_place.cpp
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

#include "unit_place.h"

#include "../object/unit.h"

namespace quadwar_app::action {
  unit_place::unit_place(sl::index id_unit) {
    m_id_unit = id_unit;
  }

  void unit_place::perform(engine::access::world w) const {
    object::unit::place_footprint(w, m_id_unit);
  }
}

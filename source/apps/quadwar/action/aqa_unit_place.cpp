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

namespace quadwar_app::action {
  namespace access = engine::access;

  unit_place::unit_place(size_t id_unit) {
    m_id_unit = id_unit;
  }

  void unit_place::perform(access::world w) const {
  }
}

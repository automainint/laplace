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

#include "../object/pathmap.h"
#include "../object/root.h"
#include "../object/sets.h"
#include "../object/unit.h"
#include <algorithm>

namespace quadwar_app::action {
  namespace access = engine::access;
  namespace sets   = object::sets;

  using object::root, object::pathmap, object::unit, engine::intval,
      engine::vec2i;

  unit_place::unit_place(sl::index id_unit) {
    m_id_unit = id_unit;
  }

  void unit_place::perform(access::world w) const {
    if constexpr (pathmap::resolution == 0) {
      error_("Invalid pathmap resolution.", __FUNCTION__);
      return;
    }

    auto u    = w.get_entity(m_id_unit);
    auto r    = w.get_entity(w.get_root());
    auto path = w.get_entity(root::get_pathmap(r));

    const auto sx = unit::scale_of_x(u) / pathmap::resolution;
    const auto sy = unit::scale_of_y(u) / pathmap::resolution;
    const auto sr = unit::scale_of_radius(u) / pathmap::resolution;

    if (sx == 0 || sy == 0 || sr == 0) {
      error_("Invalid scale.", __FUNCTION__);
      return;
    }

    const auto x0   = unit::get_x(u) / sx;
    const auto y0   = unit::get_y(u) / sy;
    const auto s    = unit::get_radius(u) / sr;
    const auto side = s * 2 + 1;

    const auto footprint = sl::vector<int8_t>(side * side, 1);

    auto p = pathmap::place(
        path, { x0 - s, y0 - s }, { side, side }, footprint);

    unit::set_position(u, { (p.x() + s) * sx, (p.y() + s) * sy });

    path.adjust();
    u.adjust();
  }
}

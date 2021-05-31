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

    constexpr sl::whole distance = 20;

    auto gen_order = [distance]() -> sl::vector<vec2i> {
      auto v = sl::vector<vec2i>((distance * 2 + 1) *
                                 (distance * 2 + 1));

      for (sl::index i = -distance, k = 0; i <= distance; i++) {
        for (sl::index j = -distance; j <= distance; j++, k++)
          v[k] = vec2i { i, j };
      }

      constexpr auto cmp = [](const vec2i a, const vec2i b) -> bool {
        const auto alen = math::square_length(a);
        const auto blen = math::square_length(b);

        if (alen < blen)
          return true;

        if (alen == blen) {
          if (a.y() < b.y())
            return true;
          if (a.y() == b.y() && a.x() < b.x())
            return true;
        }

        return false;
      };

      std::sort(v.begin(), v.end(), cmp);
      return v;
    };

    const auto order     = gen_order();
    const auto footprint = sl::vector<int8_t>(side * side, 1);

    for (sl::index i = 0; i < order.size(); i++) {
      const auto x = x0 + order[i].x() * s - s;
      const auto y = y0 + order[i].y() * s - s;

      if (pathmap::check(path, x, y, side, side, footprint)) {
        pathmap::add(path, x, y, side, side, footprint);
        unit::set_position(u, { (x + s) * sx, (y + s) * sy });
        path.adjust();
        u.adjust();
        return;
      }
    }
  }
}

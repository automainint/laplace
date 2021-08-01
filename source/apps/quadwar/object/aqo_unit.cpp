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

#include "unit.h"

#include "../../../laplace/core/utils.h"
#include "../../../laplace/engine/eval/integral.h"
#include "landscape.h"
#include "pathmap.h"
#include "player.h"
#include "root.h"

namespace quadwar_app::object {
  namespace access = engine::access;
  namespace eval   = engine::eval;

  using std::vector, engine::intval, engine::vec2i,
      engine::id_undefined, std::shared_ptr, std::make_shared;

  const engine::intval unit::default_health           = 100;
  const engine::intval unit::default_radius           = 1000;
  const engine::intval unit::default_collision_radius = 900;
  const engine::intval unit::default_movement_speed   = 200;

  sl::index unit::n_health           = {};
  sl::index unit::n_radius           = {};
  sl::index unit::n_collision_radius = {};
  sl::index unit::n_movement_speed   = {};
  sl::index unit::n_actor            = {};
  sl::index unit::n_color            = {};
  sl::index unit::n_x                = {};
  sl::index unit::n_y                = {};
  sl::index unit::n_move_x           = {};
  sl::index unit::n_move_y           = {};
  sl::index unit::n_waypoint_index   = {};
  sl::index unit::n_waypoint_count   = {};

  unit unit::m_proto(unit::proto);

  unit::unit(proto_tag) : basic_entity(1) {
    setup_sets(
        { { .id = sets::unit_health, .scale = sets::scale_points },
          { .id = sets::unit_radius, .scale = sets::scale_real },
          { .id    = sets::unit_collision_radius,
            .scale = sets::scale_real },
          { .id = sets::unit_movement_speed, .scale = sets::scale_real },
          { .id = sets::unit_actor, .scale = 1 },
          { .id = sets::unit_color, .scale = 1 },
          { .id = sets::unit_x, .scale = sets::scale_real },
          { .id = sets::unit_y, .scale = sets::scale_real },
          { .id = sets::unit_move_x, .scale = sets::scale_real },
          { .id = sets::unit_move_y, .scale = sets::scale_real },
          { .id = sets::unit_waypoint_index, .scale = 1 },
          { .id = sets::unit_waypoint_count, .scale = 1 } });

    n_health           = index_of(sets::unit_health);
    n_radius           = index_of(sets::unit_radius);
    n_collision_radius = index_of(sets::unit_collision_radius);
    n_movement_speed   = index_of(sets::unit_movement_speed);
    n_actor            = index_of(sets::unit_actor);
    n_color            = index_of(sets::unit_color);
    n_x                = index_of(sets::unit_x);
    n_y                = index_of(sets::unit_y);
    n_move_x           = index_of(sets::unit_move_x);
    n_move_y           = index_of(sets::unit_move_y);
    n_waypoint_index   = index_of(sets::unit_waypoint_index);
    n_waypoint_count   = index_of(sets::unit_waypoint_count);
  }

  unit::unit() : basic_entity(dummy) {
    *this = m_proto;
  }

  void unit::tick(access::world w) {
    const auto scale = sets::scale_real / pathmap::resolution;

    if (scale == 0) {
      error_("Invalid scale.", __FUNCTION__);
      return;
    }

    const auto ox0 = get(n_x);
    const auto oy0 = get(n_y);

    const auto dx = get(n_move_x) - ox0;
    const auto dy = get(n_move_y) - oy0;

    if (dx == 0 && dy == 0) {
      return;
    }

    const auto d = [dx, dy]() {
      const sl::index k = 100;

      const auto x = eval::div(dx, k, 1);
      const auto y = eval::div(dy, k, 1);

      return eval::sqrt(eval::mul(x, x, sets::scale_real / k) +
                            eval::mul(y, y, sets::scale_real / k),
                        sets::scale_real / k) *
             k;
    }();

    const auto v = std::min(d, get(n_movement_speed));

    const auto ox1 = eval::div(ox0 * d + dx * v, d, 1);
    const auto oy1 = eval::div(oy0 * d + dy * v, d, 1);

    const auto x0 = eval::div(ox0, scale, 1);
    const auto y0 = eval::div(oy0, scale, 1);

    const auto x1 = eval::div(ox1, scale, 1);
    const auto y1 = eval::div(oy1, scale, 1);

    if (x0 == x1 && y0 == y1) {
      set(n_x, ox1);
      set(n_y, oy1);
      return;
    }

    auto r    = w.get_entity(w.get_root());
    auto path = w.get_entity(root::get_pathmap(r));

    const auto s    = eval::div(get(n_collision_radius), scale, 1);
    const auto size = s * 2 + 1;

    const auto footprint = sl::vector<int8_t>(size * size, 1);

    if (pathmap::check_move(path, { x0, y0 }, { x1, y1 },
                            { size, size }, footprint)) {

      pathmap::subtract(path, { x0, y0 }, { size, size }, footprint);
      pathmap::add(path, { x1, y1 }, { size, size }, footprint);

      set(n_x, ox1);
      set(n_y, oy1);

    } else {
      set(n_move_x, ox0);
      set(n_move_y, oy0);
    }
  }

  auto unit::spawn_start_units(world w, sl::whole unit_count)
      -> vector<sl::index> {
    auto ids = vector<sl::index> {};

    auto r     = w.get_entity(w.get_root());
    auto land  = w.get_entity(root::get_landscape(r));
    auto slots = w.get_entity(root::get_slots(r));
    auto units = w.get_entity(root::get_units(r));

    auto locs = landscape::get_start_locs(land);

    if (!land.exist()) {
      error_("No landscape.", __FUNCTION__);
      return {};
    }

    auto player_count = slots.vec_get_size();

    if (locs.size() < player_count) {
      if (locs.empty())
        error_("No start locations.", __FUNCTION__);
      else
        error_("Invalid start locations.", __FUNCTION__);
      return {};
    }

    ids.reserve(unit_count * player_count);

    auto create_unit = [&r, &locs,
                        &slots](const sl::index id_actor,
                                const sl::index color, const intval x,
                                const intval y) -> shared_ptr<unit> {
      auto u = unit {};

      u.init(n_health, default_health * sets::scale_points);
      u.init(n_radius, default_radius);
      u.init(n_collision_radius, default_collision_radius);
      u.init(n_movement_speed, default_movement_speed);
      u.init(n_actor, id_actor);
      u.init(n_color, color);
      u.init(n_x, x * sets::scale_real);
      u.init(n_y, y * sets::scale_real);
      u.init(n_move_x, x * sets::scale_real);
      u.init(n_move_y, y * sets::scale_real);

      return make_shared<unit>(u);
    };

    for (sl::index i = 0; i < player_count; i++) {
      const auto id_actor = as_index(slots.vec_get(i));
      const auto color    = player::get_index(w.get_entity(id_actor));

      const auto x = locs[i].x();
      const auto y = locs[i].y();

      for (sl::index j = 0; j < unit_count; j++) {
        const auto id = w.spawn(create_unit(id_actor, color, x, y),
                                id_undefined);
        ids.emplace_back(id);
      }
    }

    for (sl::index i = 0; i < ids.size(); i++) {
      units.vec_add_sorted(ids[i]);
    }

    return ids;
  }

  void unit::place_footprint(world w, sl::index id_unit) {
    if (pathmap::resolution == 0) {
      error_("Invalid pathmap resolution.", __FUNCTION__);
      return;
    }

    const auto scale = sets::scale_real / pathmap::resolution;

    if (scale == 0) {
      error_("Invalid scale.", __FUNCTION__);
      return;
    }

    auto r    = w.get_entity(w.get_root());
    auto u    = w.get_entity(id_unit);
    auto path = w.get_entity(root::get_pathmap(r));

    const auto x0   = eval::div(u.get(n_x), scale, 1);
    const auto y0   = eval::div(u.get(n_y), scale, 1);
    const auto s    = eval::div(u.get(n_collision_radius), scale, 1);
    const auto side = s * 2 + 1;

    const auto footprint = sl::vector<int8_t>(side * side, 1);

    auto p = pathmap::place(path, { x0, y0 }, { side, side },
                            footprint);

    u.set(n_x, p.x() * scale);
    u.set(n_y, p.y() * scale);
    u.set(n_move_x, p.x() * scale);
    u.set(n_move_y, p.y() * scale);
    u.adjust();

    path.adjust();
  }

  void unit::order_move(world w, sl::index id_actor,
                        sl::index id_unit, vec2i target) {

    auto u = w.get_entity(id_unit);

    if (get_actor(u) != id_actor) {
      return;
    }

    u.set(n_move_x, target.x());
    u.set(n_move_y, target.y());
    u.adjust();
  }

  auto unit::get_actor(entity en) -> sl::index {
    return as_index(en.get(n_actor));
  }

  auto unit::get_color(entity en) -> sl::index {
    return as_index(en.get(n_color));
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

  auto unit::get_radius(entity en) -> intval {
    return en.get(n_radius);
  }

  auto unit::get_health(entity en) -> intval {
    return en.get(n_health);
  }

  auto unit::get_position_scaled(entity en) -> view::vec2 {
    return { static_cast<view::real>(get_x(en)) / sets::scale_real,
             static_cast<view::real>(get_y(en)) / sets::scale_real };
  }

  auto unit::get_radius_scaled(entity en) -> view::real {
    return static_cast<view::real>(get_radius(en)) / sets::scale_real;
  }
}

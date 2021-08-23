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

  using std::min, std::max, std::span, std::vector, engine::intval,
      engine::vec2i, engine::vec2z, engine::id_undefined,
      std::shared_ptr, std::make_shared;

  const engine::intval unit::default_health           = 100;
  const engine::intval unit::default_radius           = 1200;
  const engine::intval unit::default_collision_radius = 600;
  const engine::intval unit::default_movement_speed   = 200;

  sl::index unit::n_health           = {};
  sl::index unit::n_radius           = {};
  sl::index unit::n_collision_radius = {};
  sl::index unit::n_movement_speed   = {};
  sl::index unit::n_actor            = {};
  sl::index unit::n_color            = {};
  sl::index unit::n_x                = {};
  sl::index unit::n_y                = {};
  sl::index unit::n_target_order     = {};
  sl::index unit::n_target_x         = {};
  sl::index unit::n_target_y         = {};

  unit unit::m_proto(unit::proto);

  unit::unit(proto_tag) : basic_entity(1) {
    setup_sets(
        { { .id = sets::unit_health, .scale = sets::scale_points },
          { .id = sets::unit_radius, .scale = sets::scale_real },
          { .id    = sets::unit_collision_radius,
            .scale = sets::scale_real },
          { .id    = sets::unit_movement_speed,
            .scale = sets::scale_real },
          { .id = sets::unit_actor, .scale = 1 },
          { .id = sets::unit_color, .scale = 1 },
          { .id = sets::unit_x, .scale = sets::scale_real },
          { .id = sets::unit_y, .scale = sets::scale_real },
          { .id = sets::unit_target_order, .scale = 1 },
          { .id = sets::unit_target_x, .scale = sets::scale_real },
          { .id = sets::unit_target_y, .scale = sets::scale_real } });

    n_health           = index_of(sets::unit_health);
    n_radius           = index_of(sets::unit_radius);
    n_collision_radius = index_of(sets::unit_collision_radius);
    n_movement_speed   = index_of(sets::unit_movement_speed);
    n_actor            = index_of(sets::unit_actor);
    n_color            = index_of(sets::unit_color);
    n_x                = index_of(sets::unit_x);
    n_y                = index_of(sets::unit_y);
    n_target_order     = index_of(sets::unit_target_order);
    n_target_x         = index_of(sets::unit_target_x);
    n_target_y         = index_of(sets::unit_target_y);
  }

  unit::unit() : basic_entity(dummy) {
    *this = m_proto;
  }

  void unit::tick(access::world w) {
    auto r   = w.get_entity(w.get_root());
    auto map = w.get_entity(root::get_pathmap(r));

    do_search(map);
    do_movement(map);
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

    auto create_unit =
        [&](const sl::index id_actor, const sl::index color,
            const intval x, const intval y) -> shared_ptr<unit> {
      auto u = unit {};

      u.init(n_health, default_health * sets::scale_points);
      u.init(n_radius, default_radius);
      u.init(n_collision_radius, default_collision_radius);
      u.init(n_movement_speed, default_movement_speed);
      u.init(n_actor, id_actor);
      u.init(n_color, color);
      u.init(n_x, x * sets::scale_real);
      u.init(n_y, y * sets::scale_real);

      return make_shared<unit>(u);
    };

    for (sl::index i = 0; i < player_count; i++) {
      const auto id_actor = as_index(slots.vec_get(i));
      const auto color    = player::get_index(w.get_entity(id_actor));

      const auto x = locs[i].x();
      const auto y = locs[i].y();

      for (sl::index j = 0; j < unit_count; j++) {
        const auto id = w.spawn(
            create_unit(id_actor, color, x, y), id_undefined);
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

    const auto x0    = as_index(eval::div(u.get(n_x), scale, 1));
    const auto y0    = as_index(eval::div(u.get(n_y), scale, 1));
    const auto r_max = as_index(eval::div(u.get(n_radius), scale, 1));
    const auto r_min = as_index(
        eval::div(u.get(n_collision_radius), scale, 1));

    const auto foot_max = make_footprint(r_max);
    const auto foot_min = make_footprint(r_min);

    auto p = pathmap::find_empty(
        path, { x0, y0 }, foot_max.size, foot_max.bytes);
    pathmap::add(path, p, foot_min.size, foot_min.bytes);

    u.set(n_x, p.x() * scale);
    u.set(n_y, p.y() * scale);

    path.adjust();
    u.adjust();
  }

  void unit::order_move(
      world w, sl::index id_actor, sl::index id_unit, vec2i target) {

    auto u = w.get_entity(id_unit);

    if (get_actor(u) != id_actor) {
      return;
    }

    u.set(n_target_order, 1);
    u.set(n_target_x, target.x());
    u.set(n_target_y, target.y());
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

  auto unit::make_footprint(sl::whole radius) -> footprint_data {
    const auto size = 1 + radius * 2;

    return { .size   = { size, size },
             .center = { radius, radius },
             .bytes  = sl::vector<int8_t>(size * size, 1) };
  }

  void unit::do_search(entity map) noexcept {
    if (get(n_target_order) > 0) {
      if (pathmap::resolution == 0) {
        error_("Invalid pathmap resolution.", __FUNCTION__);
        return;
      }

      const auto scale = sets::scale_real / pathmap::resolution;

      const auto x0 = as_index(eval::div(get(n_x), scale, 1));
      const auto y0 = as_index(eval::div(get(n_y), scale, 1));

      const auto x1 = as_index(eval::div(get(n_target_x), scale, 1));
      const auto y1 = as_index(eval::div(get(n_target_y), scale, 1));

      const auto width  = as_index(pathmap::get_width(map));
      const auto height = as_index(pathmap::get_height(map));
      const auto size   = vec2z { width, height };

      const auto radius = as_index(
          eval::div(get(n_radius), scale, 1));

      const auto footprint = make_footprint(radius);

      const auto src = [&]() {
        auto v = map.bytes_get_all();

        const auto cx = footprint.center.x();
        const auto cy = footprint.center.y();

        const auto sx = footprint.size.x();
        const auto sy = footprint.size.y();

        const auto px0 = x0 - cx;
        const auto py0 = y0 - cy;

        const auto i0 = max<sl::index>(0, px0) - px0;
        const auto j0 = max<sl::index>(0, py0) - py0;
        const auto i1 = min<sl::index>(width, px0 + sx) - px0;
        const auto j1 = min<sl::index>(height, py0 + sy) - py0;

        for (sl::index j = j0; j < j1; j++)
          for (sl::index i = i0; i < i1; i++) {
            v[(py0 + j) * width + px0 + i] -=
                footprint.bytes[j * sx + i];
          }

        return v;
      }();

      m_pathmap.resize(width * height);
      for (auto &x : m_pathmap) { x = 0; }

      eval::grid::convolve(
          { width, height }, m_pathmap, src, footprint.size,
          footprint.center, footprint.bytes);

      m_destination = { x1, y1 };

      m_search = eval::grid::path_search_init(
          size, 16, m_pathmap, [](const int8_t x) { return x <= 0; },
          { x0, y0 }, m_destination);

      m_searching = true;
      m_movement  = true;

      set(n_target_order, 0);
    }

    if (!m_searching) {
      return;
    }

    for (sl::index i = 0; i < 10; i++)
      if (eval::grid::path_search_loop(m_search) !=
          eval::astar::status::progress) {
        m_searching = false;
        break;
      }

    const auto points = eval::grid::path_search_finish(m_search);

    if (m_current >= m_waypoints.size()) {
      m_waypoints = points;
      m_current   = 1;
      return;
    }

    /*for (sl::index i = 0; i < m_current; i++) {
      if (i >= m_waypoints.size() || i >= points.size()) {
        break;
      }

      if (m_waypoints[i] != m_waypoints[m_current]) {
        m_current = i;
        break;
      }
    }*/

    m_waypoints = points;
  }

  void unit::do_movement(entity map) noexcept {
    if (!m_movement) {
      return;
    }

    if (pathmap::resolution == 0) {
      error_("Invalid pathmap resolution.", __FUNCTION__);
      return;
    }

    const auto scale = sets::scale_real / pathmap::resolution;

    auto delta = get(n_movement_speed);

    const auto ox0 = get(n_x);
    const auto oy0 = get(n_y);

    auto ox = ox0;
    auto oy = oy0;

    while (delta > 0) {
      if (m_current >= m_waypoints.size()) {
        m_movement = m_searching;
        break;
      }

      const auto ox1 = m_waypoints[m_current].x() * scale;
      const auto oy1 = m_waypoints[m_current].y() * scale;

      const auto odx = ox1 - ox;
      const auto ody = oy1 - oy;

      const auto dx = eval::div(odx, scale, 1);
      const auto dy = eval::div(ody, scale, 1);

      if (dx == 0 && dy == 0) {
        m_current++;
        continue;
      }

      const auto distance = eval::sqrt(odx * odx + ody * ody, 1);

      if (delta < distance) {
        ox = eval::div(ox * distance + odx * delta, distance, 1);
        oy = eval::div(oy * distance + ody * delta, distance, 1);
        break;
      }

      ox = ox1;
      oy = oy1;

      delta -= distance;
    }

    if (ox != ox0 || oy != oy0) {
      const auto radius = as_index(
          eval::div(get(n_collision_radius), scale, 1));

      const auto foot = make_footprint(radius);

      const auto x0 = as_index(eval::div(ox0, scale, 1));
      const auto y0 = as_index(eval::div(oy0, scale, 1));

      const auto x = as_index(eval::div(ox, scale, 1));
      const auto y = as_index(eval::div(oy, scale, 1));

      if (x0 != x || y0 != y) {
        if (!pathmap::check_move(
                map, { x0, y0 }, { x, y }, foot.size, foot.bytes)) {
          m_searching = false;
          m_movement  = false;
          return;
        }

        pathmap::subtract(map, { x0, y0 }, foot.size, foot.bytes);
        pathmap::add(map, { x, y }, foot.size, foot.bytes);
      }

      set(n_x, ox);
      set(n_y, oy);
    }
  }
}

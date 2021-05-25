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

#include "../action/unit_move.h"
#include "landscape.h"
#include "pathmap.h"
#include "root.h"

namespace quadwar_app::object {
  namespace access = engine::access;

  using std::vector, engine::intval, engine::vec2i,
      engine::id_undefined, std::make_shared, action::unit_move;

  sl::index unit::n_actor  = {};
  sl::index unit::n_x      = {};
  sl::index unit::n_y      = {};
  sl::index unit::n_radius = {};
  sl::index unit::n_health = {};

  unit unit::m_proto(unit::proto);

  unit::unit(proto_tag) : basic_entity(1) {
    setup_sets(
        { { .id = sets::unit_actor, .scale = 1 },
          { .id = sets::unit_x, .scale = sets::scale_real },
          { .id = sets::unit_y, .scale = sets::scale_real },
          { .id = sets::unit_radius, .scale = pathmap::resolution },
          { .id = sets::unit_health, .scale = sets::scale_points } });

    n_actor  = index_of(sets::unit_actor);
    n_x      = index_of(sets::unit_x);
    n_y      = index_of(sets::unit_y);
    n_radius = index_of(sets::unit_radius);
    n_health = index_of(sets::unit_health);
  }

  unit::unit() : basic_entity(dummy) {
    *this = m_proto;
  }

  void unit::tick(access::world w) { }

  auto unit::spawn_start_units(world w, sl::whole unit_count)
      -> vector<sl::index> {

    auto ids = vector<sl::index> {};

    auto r    = w.get_entity(w.get_root());
    auto land = w.get_entity(root::get_landscape(r));
    auto locs = landscape::get_start_locs(land);

    if (!land.exist()) {
      error_("No landscape.", __FUNCTION__);
      return {};
    }

    auto player_count = root::get_slot_count(r);

    if (locs.size() < player_count) {
      if (locs.empty())
        error_("No start locations.", __FUNCTION__);
      else
        error_("Invalid start locations.", __FUNCTION__);
      return {};
    }

    ids.reserve(unit_count * player_count);

    for (sl::index i = 0; i < player_count; i++) {
      const auto u = [&r, &locs, &i]() -> unit {
        auto u = unit {};

        const auto id_actor = root::get_slot(r, i);

        const auto x      = locs[i].x();
        const auto y      = locs[i].y();
        const auto radius = default_radius;
        const auto health = default_health;

        const auto sx = u.scale_of(n_x);
        const auto sy = u.scale_of(n_y);
        const auto sh = u.scale_of(n_health);

        u.init(n_actor, id_actor);
        u.init(n_x, x * sx);
        u.init(n_y, y * sy);
        u.init(n_radius, radius);
        u.init(n_health, health * sh);

        return u;
      }();

      for (sl::index j = 0; j < unit_count; j++) {
        const auto id = w.spawn(make_shared<unit>(u), id_undefined);
        root::unit_create(r, id);
        ids.emplace_back(id);
      }
    }

    return ids;
  }

  void unit::set_actor(entity en, sl::index id_actor) {
    en.set(n_actor, static_cast<int64_t>(id_actor));
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

  void unit::set_radius(entity en, intval radius) {
    en.set(n_radius, radius);
  }

  void unit::set_health(entity en, intval health) {
    en.set(n_health, health);
  }

  auto unit::get_actor(entity en) -> sl::index {
    return as_index(en.get(n_actor));
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

  auto unit::scale_of_x(entity en) -> engine::intval {
    return en.scale_of(n_x);
  }

  auto unit::scale_of_y(entity en) -> engine::intval {
    return en.scale_of(n_y);
  }

  auto unit::scale_of_radius(entity en) -> engine::intval {
    return en.scale_of(n_radius);
  }

  auto unit::get_position_scaled(entity en) -> view::vec2 {
    const auto sx = en.scale_of(n_x);
    const auto sy = en.scale_of(n_y);

    if (sx == 0 || sy == 0)
      return {};

    return { static_cast<view::real>(get_x(en)) / sx,
             static_cast<view::real>(get_y(en)) / sy };
  }
  auto unit::get_radius_scaled(entity en) -> view::real {
    const auto s = en.scale_of(n_radius);

    if (s == 0)
      return {};

    return static_cast<view::real>(get_radius(en)) / s;
  }
}

/*  apps/quadwar/object/aqo_landscape.cpp
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

#include "../../../laplace/core/serial.h"
#include "../../../laplace/engine/eval/maze.h"
#include "landscape.h"
#include "root.h"

namespace quadwar_app::object {
  namespace access = engine::access;
  namespace maze   = engine::eval::maze;

  using engine::basic_entity, engine::vec2z, std::min, std::span;

  sl::index landscape::n_version = 0;
  sl::index landscape::n_width   = 0;
  sl::index landscape::n_height  = 0;

  landscape landscape::m_proto(landscape::proto);

  landscape::landscape(proto_tag) {

    setup_sets({ { .id = sets::state_version, .scale = 1 },
                 { .id = sets::land_width, .scale = 1 },
                 { .id = sets::land_height, .scale = 1 } });

    n_version = index_of(sets::state_version);
    n_width   = index_of(sets::land_width);
    n_height  = index_of(sets::land_height);
  }

  landscape::landscape() : basic_entity(dummy) {
    *this = m_proto;
  }

  auto landscape::gen_maze(world w, sl::index width, sl::index height,
                           span<const vec2z> start_locs) noexcept
      -> sl::vector<int8_t> {

    const auto cols = ((width + cave_size / 2) / cave_size) | 1;
    const auto rows = ((height + cave_size / 2) / cave_size) | 1;

    auto map = sl::vector<int8_t>(cols * rows);
    auto uni = std::uniform_int_distribution<uint64_t> {};

    maze::generate({ cols, rows }, map, [&w, &uni]() {
      return w.random(uni);
    });

    for (const auto &p : start_locs) {
      const auto x = (p.x() * cols) / width;
      const auto y = (p.y() * rows) / height;

      map[(y - 1) * rows + x - 1] = tile_walkable;
      map[(y + 1) * rows + x - 1] = tile_walkable;
      map[(y - 1) * rows + x + 1] = tile_walkable;
      map[(y + 1) * rows + x + 1] = tile_walkable;
      map[(y - 1) * rows + x]     = tile_walkable;
      map[(y + 1) * rows + x]     = tile_walkable;
      map[y * rows + x - 1]       = tile_walkable;
      map[y * rows + x + 1]       = tile_walkable;
      map[y * rows + x]           = tile_walkable;
    }

    auto big = sl::vector<int8_t>(width * height);

    maze::stretch({ width, height }, big, { cols, rows }, map,
                  tunnel_size, gate_size);

    return big;
  }

  auto landscape::gen_start_locs(sl::whole width, sl::whole height,
                                 sl::whole edge,
                                 sl::whole player_count) noexcept
      -> sl::vector<vec2z> {

    if (player_count == 0)
      return {};

    auto delta = ((width - edge * 2) * 2 + (height - edge * 2) * 2) /
                 player_count;

    if (delta <= 0)
      return {};

    auto locs = sl::vector<vec2z> {};

    auto x = edge;
    auto y = edge;
    auto p = sl::index {};

    while (p < player_count && x + delta + edge <= width) {
      auto v = vec2z {};
      v.x()  = x;
      v.y()  = y;

      locs.emplace_back(v);

      x += delta;
      p++;
    }

    x = width - edge - 1;

    while (p < player_count && y + delta + edge <= height) {
      auto v = vec2z {};
      v.x()  = x;
      v.y()  = y;

      locs.emplace_back(v);

      y += delta;
      p++;
    }

    x = edge;
    y = edge + delta;

    while (p < player_count && y + delta + edge <= height) {
      auto v = vec2z {};
      v.x()  = x;
      v.y()  = y;

      locs.emplace_back(v);

      y += delta;
      p++;
    }

    y = height - edge - 1;

    while (p < player_count && x + delta + edge <= width) {
      auto v = vec2z {};
      v.x()  = x;
      v.y()  = y;

      locs.emplace_back(v);

      x += delta;
      p++;
    }

    if (p < player_count) {
      auto v = vec2z {};
      v.x()  = width - edge - 1;
      v.y()  = height - edge - 1;

      locs.emplace_back(v);
    }

    return locs;
  }

  void landscape::create_maze(world w, sl::index width,
                              sl::index height, sl::index player_count) {

    auto r    = w.get_entity(w.get_root());
    auto land = std::make_shared<object::landscape>();

    const auto locs = gen_start_locs(width, height, cave_size * 2,
                                     player_count);

    const auto tiles = gen_maze(w, width, height, locs);

    land->init(n_width, width);
    land->init(n_height, height);
    land->init(n_version, land->get(n_version) + 1);

    land->bytes_resize(tiles.size());

    for (sl::index i = 0; i < tiles.size(); i++) {
      land->bytes_init(i, tiles[i]);
    }

    land->vec_resize(locs.size() * 2);

    for (sl::index i = 0; i < locs.size(); i++) {
      land->vec_init(i * 2, locs[i].x());
      land->vec_init(i * 2 + 1, locs[i].y());
    }

    object::root::set_landscape(r, w.spawn(land, engine::id_undefined));

    r.adjust();
  }

  auto landscape::get_version(entity en) -> sl::index {
    return static_cast<sl::index>(en.get(n_version));
  }

  auto landscape::get_width(entity en) -> sl::index {
    return static_cast<sl::index>(en.get(n_width));
  }

  auto landscape::get_height(entity en) -> sl::index {
    return static_cast<sl::index>(en.get(n_height));
  }

  auto landscape::get_tile(entity en, sl::index x, sl::index y)
      -> int8_t {

    return en.bytes_get(y * get_width(en) + x);
  }

  auto landscape::get_start_loc(entity en, sl::index index) -> vec2z {

    return { en.vec_get(index * 2), en.vec_get(index * 2 + 1) };
  }

  auto landscape::get_tiles(entity en) -> sl::vector<int8_t> {
    auto tiles = sl::vector<int8_t>(en.bytes_get_size());

    for (sl::index i = 0; i < tiles.size(); i++) {
      tiles[i] = en.bytes_get(i);
    }

    return tiles;
  }

  auto landscape::get_start_locs(entity en) -> sl::vector<vec2z> {
    auto locs = sl::vector<vec2z>(en.vec_get_size() / 2);

    for (sl::index i = 0; i < locs.size(); i++) {
      locs[i].x() = en.vec_get(i * 2);
      locs[i].y() = en.vec_get(i * 2 + 1);
    }

    return locs;
  }

  void landscape::inc_version(entity en) {
    en.apply_delta(n_version, 1);
  }

  void landscape::set_tile(entity en, sl::index x, sl::index y,
                           int8_t tile) {

    en.bytes_set(y * get_width(en) + x, tile);
  }

  void landscape::set_start_loc(entity en, sl::index n,
                                vec2z start_loc) {

    en.vec_set(n * 2, start_loc.x());
    en.vec_set(n * 2 + 1, start_loc.y());
  }

  void landscape::set_tiles(entity en, span<const int8_t> tiles) {

    en.bytes_write(0, tiles);
  }

  void landscape::set_size(entity en, sl::whole width,
                           sl::whole height) {

    en.set(n_width, width);
    en.set(n_height, height);

    en.bytes_resize(width * height);
  }

  void landscape::set_start_locs(entity en, span<const vec2z> locs) {

    en.vec_resize(locs.size() * 2);

    for (sl::index i = 0; i < locs.size(); i++) {
      en.vec_set(i * 2, locs[i].x());
      en.vec_set(i * 2 + 1, locs[i].y());
    }
  }
}

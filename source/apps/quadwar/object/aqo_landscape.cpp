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

  using engine::basic_entity, engine::vec2z, std::vector, std::min,
      std::span;

  size_t landscape::n_version = 0;
  size_t landscape::n_width   = 0;
  size_t landscape::n_height  = 0;

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

  auto landscape::gen_maze(world w, size_t width, size_t height,
                           span<const vec2z> start_locs) noexcept
      -> vector<uint8_t> {

    const auto cols = ((width + cave_size / 2) / cave_size) | 1;
    const auto rows = ((height + cave_size / 2) / cave_size) | 1;

    auto map = vector<uint8_t>(cols * rows);
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

    auto big = vector<uint8_t>(width * height);

    maze::stretch({ width, height }, big, { cols, rows }, map,
                  tunnel_size, gate_size);

    return big;
  }

  auto landscape::gen_start_locs(sl::whole width, sl::whole height,
                                 sl::whole edge,
                                 sl::whole player_count) noexcept
      -> vector<vec2z> {

    if (player_count == 0)
      return {};

    auto delta = ((width - edge * 2) * 2 + (height - edge * 2) * 2) /
                 player_count;

    if (delta <= 0)
      return {};

    auto locs = vector<vec2z> {};

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

  void landscape::create_maze(world w, size_t width, size_t height,
                              size_t player_count) {
    auto r    = w.get_entity(w.get_root());
    auto land = std::make_shared<object::landscape>();

    auto locs = gen_start_locs(
        width, height, cave_size * 2, player_count);

    set_size({ land, access::sync }, width, height);
    set_tiles({ land, access::sync }, gen_maze(w, width, height, locs));
    set_start_locs({ land, access::sync }, locs);
    inc_version({ land, access::async });

    object::root::set_landscape(r, w.spawn(land, engine::id_undefined));
  }

  auto landscape::get_version(entity en) -> size_t {
    return static_cast<size_t>(en.get(n_version));
  }

  auto landscape::get_width(entity en) -> size_t {
    return static_cast<size_t>(en.get(n_width));
  }

  auto landscape::get_height(entity en) -> size_t {
    return static_cast<size_t>(en.get(n_height));
  }

  auto landscape::get_tile(entity en, size_t x, size_t y) -> uint8_t {

    const auto tile = en.request(
        sets::land_get_tile, serial::pack_to_array(x, y));

    return serial::rd<uint8_t>(tile, 0);
  }

  auto landscape::get_start_loc(entity en, size_t index) -> vec2z {
    const auto loc = en.request(
        sets::land_get_start_loc, serial::pack_to_array(index));

    return { serial::rd<size_t>(loc, 0),
             serial::rd<size_t>(loc, sizeof(size_t)) };
  }

  auto landscape::get_tiles(entity en) -> vector<uint8_t> {
    return en.request(sets::land_get_tiles);
  }

  auto landscape::get_start_locs(entity en) -> vector<vec2z> {
    const auto locs = en.request(sets::land_get_start_locs);

    auto v = vector<vec2z>(locs.size() / (sizeof(size_t) * 2));

    for (size_t i = 0; i < v.size(); i++) {
      v[i] = vec2z { serial::rd<size_t>(locs, sizeof(size_t) * (i * 2)),
                     serial::rd<size_t>(
                         locs, sizeof(size_t) * (i * 2 + 1)) };
    }

    return v;
  }

  void landscape::inc_version(entity en) {
    en.apply_delta(n_version, 1);
  }

  void landscape::set_tile(entity en, size_t x, size_t y, uint8_t tile) {

    en.modify(sets::land_set_tile, serial::pack_to_array(x, y, tile));
  }

  void landscape::set_start_loc(entity en, size_t index,
                                vec2z start_loc) {

    en.modify(
        sets::land_set_start_loc,
        serial::pack_to_array(index, start_loc.x(), start_loc.y()));
  }

  void landscape::set_tiles(entity en, const vector<uint8_t> &tiles) {

    en.modify(sets::land_set_tiles,
              serial::pack_to_bytes(span_cbyte(tiles)));
  }

  void landscape::set_size(entity en, size_t width, size_t height) {

    en.modify(
        sets::land_set_size, serial::pack_to_array(width, height));
  }
  void landscape::set_start_locs(entity en, span<const vec2z> locs) {

    auto v = vuint(locs.size() * 2);

    for (size_t i = 0; i < locs.size(); i++) {
      v[i * 2]     = locs[i].x();
      v[i * 2 + 1] = locs[i].y();
    }

    en.modify(sets::land_set_start_locs,
              serial::pack_to_bytes(span_cuint(v)));
  }

  auto landscape::do_request(size_t id, span_cbyte args) const
      -> vbyte {
    if (id == sets::land_get_tile) {

      return serial::pack_to_bytes(
          get_tile(serial::rd<size_t>(args, args_x),
                   serial::rd<size_t>(args, args_y)));

    } else if (id == sets::land_get_start_loc) {

      const auto index = serial::rd<size_t>(args, args_start_loc);
      const auto loc   = get_start_loc(index);

      return serial::pack_to_bytes(loc.x(), loc.y());

    } else if (id == sets::land_get_tiles) {
      return serial::pack_to_bytes(span_cbyte(m_tiles));

    } else if (id == sets::land_get_start_locs) {
      auto v = vbyte(m_start_locs.size() * sizeof(size_t) * 2);

      for (size_t i = 0; i < m_start_locs.size(); i++) {
        serial::wr<size_t>(
            v, sizeof(size_t) * (2 * i), m_start_locs[i].x());
        serial::wr<size_t>(
            v, sizeof(size_t) * (2 * i + 1), m_start_locs[i].y());
      }

      return v;
    }

    return {};
  }

  void landscape::do_modify(size_t id, span_cbyte args) {
    if (id == sets::land_set_size) {

      set_size(serial::rd<size_t>(args, args_width),
               serial::rd<size_t>(args, args_height));

    } else if (id == sets::land_set_start_loc) {

      set_start_loc(serial::rd<size_t>(args, args_start_loc),
                    { serial::rd<size_t>(args, args_start_loc_x),
                      serial::rd<size_t>(args, args_start_loc_y) });

    } else if (id == sets::land_set_start_locs) {

      m_start_locs.resize(args.size() / (sizeof(size_t) * 2));

      for (size_t i = 0; i < m_start_locs.size(); i++) {
        m_start_locs[i].x() = serial::rd<size_t>(
            args, sizeof(size_t) * (i * 2));
        m_start_locs[i].y() = serial::rd<size_t>(
            args, sizeof(size_t) * (i * 2 + 1));
      }

    } else if (id == sets::land_set_tile) {

      set_tile(serial::rd<size_t>(args, args_x),
               serial::rd<size_t>(args, args_y),
               serial::rd<uint8_t>(args, args_tile));

    } else if (id == sets::land_set_tiles) {

      const auto size = min(m_tiles.size(), args.size());
      memcpy(m_tiles.data(), args.data(), size);
    }
  }

  auto landscape::get_tile(size_t x, size_t y) const -> uint8_t {
    const auto width  = static_cast<size_t>(locked_get(n_width));
    const auto height = static_cast<size_t>(locked_get(n_height));

    if (x >= width || y >= height)
      return 0;

    size_t index = y * width + x;

    if (index >= m_tiles.size())
      return 0;

    return m_tiles[index];
  }

  auto landscape::get_start_loc(size_t index) const -> engine::vec2z {
    if (index >= m_start_locs.size())
      return {};

    return m_start_locs[index];
  }

  void landscape::set_tile(size_t x, size_t y, uint8_t value) {
    const auto width  = static_cast<size_t>(locked_get(n_width));
    const auto height = static_cast<size_t>(locked_get(n_height));

    if (x >= width || y >= height)
      return;

    size_t index = y * width + x;

    if (index >= m_tiles.size())
      return;

    m_tiles[index] = value;
  }

  void landscape::set_size(size_t width, size_t height) {
    m_tiles.resize(width * height);

    init(n_width, static_cast<int64_t>(width));
    init(n_height, static_cast<int64_t>(height));
  }

  void landscape::set_start_loc(size_t index, const engine::vec2z loc) {
    if (index < m_start_locs.size()) {
      m_start_locs[index] = loc;
    }
  }
}

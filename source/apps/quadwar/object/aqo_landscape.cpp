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

#include "../../../laplace/core/utils.h"
#include "landscape.h"
#include "root.h"

namespace quadwar_app::object {
  namespace access = engine::access;

  using engine::basic_entity, std::vector, std::span, std::min,
      std::tuple;

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

  auto landscape::path_exists(     //
      span<const int64_t> tiles,   //
      size_t width, size_t height, //
      size_t x0, size_t y0,        //
      size_t x1, size_t y1) noexcept -> bool {

    if (x0 >= width || y0 >= height || x1 >= width || y1 >= height)
      return false;

    if (tiles.size() != width * height)
      return false;

    auto path = vector<int>(tiles.size(), 0);

    path[y0 * width + x0] = 1;

    bool done = false;

    while (!done) {

      if (path[y1 * width + x1] != 0)
        return true;

      done = true;

      for (size_t j = 0; j < height; j++)
        for (size_t i = 0; i < width; i++) {

          if (path[j * width + i] == 0)
            continue;

          if (i < x1 && width && path[j * width + (i + 1)] == 0) {
            if (tiles[j * width + (i + 1)] == tile_walkable) {
              path[j * width + (i + 1)] = 1;
              done                      = false;
            }
          } else if (i > x1 && path[j * width + (i - 1)] == 0) {
            if (tiles[j * width + (i - 1)] == tile_walkable) {
              path[j * width + (i - 1)] = 1;
              done                      = false;
            }
          }

          if (j < y1 && height && path[(j + 1) * width + i] == 0) {
            if (tiles[(j + 1) * width + i] == tile_walkable) {
              path[(j + 1) * width + i] = 1;
              done                      = false;
            }
          } else if (j > y1 && path[(j - 1) * width + i] == 0) {
            if (tiles[(j - 1) * width + i] == tile_walkable) {
              path[(j - 1) * width + i] = 1;
              done                      = false;
            }
          }
        }

      if (done) {
        for (size_t j = 0; j < height; j++)
          for (size_t i = 0; i < width; i++) {

            if (path[j * width + i] == 0)
              continue;

            if (i + 1 < width && path[j * width + (i + 1)] == 0) {
              if (tiles[j * width + (i + 1)] == tile_walkable) {
                path[j * width + (i + 1)] = 1;
                done                      = false;
              }
            }

            if (i > 0 && path[j * width + (i - 1)] == 0) {
              if (tiles[j * width + (i - 1)] == tile_walkable) {
                path[j * width + (i - 1)] = 1;
                done                      = false;
              }
            }

            if (j + 1 < height && path[(j + 1) * width + i] == 0) {
              if (tiles[(j + 1) * width + i] == tile_walkable) {
                path[(j + 1) * width + i] = 1;
                done                      = false;
              }
            }

            if (j > 0 && path[(j - 1) * width + i] == 0) {
              if (tiles[(j - 1) * width + i] == tile_walkable) {
                path[(j - 1) * width + i] = 1;
                done                      = false;
              }
            }
          }
      }
    }

    return false;
  }

  auto landscape::gen_maze( //
      world  w,
      size_t width, //
      size_t height) noexcept -> vector<int64_t> {

    auto tiles = vector<int64_t>(width * height, tile_walkable);

    for (size_t i = 0; i < width; i++) {
      tiles[i]                        = tile_border;
      tiles[(height - 1) * width + i] = tile_border;
    }

    for (size_t j = 1; j + 1 < height; j++) {
      tiles[j * width]               = tile_border;
      tiles[j * width + (width - 1)] = tile_border;
    }

    const auto cols = (width + cave_size / 2) / cave_size;
    const auto rows = (height + cave_size / 2) / cave_size;

    if (cols > 0 && rows > 0) {
      const auto cave_width  = width / cols;
      const auto cave_height = height / rows;

      const auto caves_x0 = (width - cols * cave_width) / 2;
      const auto caves_y0 = (height - rows * cave_height) / 2;

      for (size_t col = 0; col < cols; col++)
        for (size_t row = 0; row < rows; row++) {
          const auto x0 = caves_x0 + col * cave_width;
          const auto y0 = caves_y0 + row * cave_height;
          const auto x1 = min(width - 1, x0 + cave_width - 1);
          const auto y1 = min(height - 1, y0 + cave_height - 1);

          for (size_t i = x0; i <= x1; i++) {
            tiles[y0 * width + i] = tile_border;
            tiles[y1 * width + i] = tile_border;
          }

          for (size_t j = y0 + 1; j < y1; j++) {
            tiles[j * width + x0] = tile_border;
            tiles[j * width + x1] = tile_border;
          }
        }

      auto rand_dir = std::uniform_int_distribution(0, 3);

      auto caves = vector<tuple<size_t, size_t>>(cols * rows);

      for (size_t j = 0; j < rows; j++) {
        for (size_t i = 0; i < cols; i++)
          caves[j * cols + i] = { i, j };
      }

      caves.resize(caves.size() * 2);

      for (size_t n = caves.size() / 2; n < caves.size(); n++) {
        caves[n] = caves[n - caves.size() / 2];
      }

      const auto limit = caves.size() / 2 + 1;

      while (caves.size() > limit) {
        auto rand_cave = std::uniform_int_distribution(
            size_t {}, caves.size() - 1);

        const auto cave = w.random(rand_cave);

        const auto dir = w.random(rand_dir);

        const auto col0 = std::get<0>(caves[cave]);
        const auto row0 = std::get<1>(caves[cave]);

        const auto [col1, row1] = [col0, row0, dir, cols, rows]() {
          if (dir == 0 && col0 + 1 < cols) {
            return tuple { col0 + 1, row0 };
          }

          if (dir == 1 && row0 + 1 < rows) {
            return tuple { col0, row0 + 1 };
          }

          if (dir == 2 && col0 > 0) {
            return tuple { col0 - 1, row0 };
          }

          if (dir == 3 && row0 > 0) {
            return tuple { col0, row0 - 1 };
          }

          if (col0 + 1 < cols) {
            return tuple { col0 + 1, row0 };
          }

          if (row0 + 1 < rows) {
            return tuple { col0, row0 + 1 };
          }

          if (col0 > 0) {
            return tuple { col0 - 1, row0 };
          }

          if (row0 > 0) {
            return tuple { col0, row0 - 1 };
          }

          return tuple { col0, row0 };
        }();

        const auto x0 = caves_x0 + cave_width / 2 + col0 * cave_width;
        const auto y0 = caves_y0 + cave_height / 2 + row0 * cave_height;
        const auto x1 = caves_x0 + cave_width / 2 + col1 * cave_width;
        const auto y1 = caves_y0 + cave_height / 2 + row1 * cave_height;

        if (!path_exists(tiles, width, height, x0, y0, x1, y1)) {
          caves.erase(caves.begin() + static_cast<ptrdiff_t>(cave));

          if (y0 == y1) {
            for (size_t y = y0 - cave_size / 4;
                 y <= y0 + cave_size / 4; y++) {
              if (x0 < x1) {
                for (size_t i = x0; i < x1; i++)
                  tiles[y * width + i] = tile_walkable;
              } else {
                for (size_t i = x1; i < x0; i++)
                  tiles[y * width + i] = tile_walkable;
              }
            }
          } else {
            for (size_t x = x0 - cave_size / 4;
                 x <= x0 + cave_size / 4; x++) {
              if (y0 < y1) {
                for (size_t j = y0; j < y1; j++)
                  tiles[j * width + x] = tile_walkable;
              } else {
                for (size_t j = y1; j < y0; j++)
                  tiles[j * width + x] = tile_walkable;
              }
            }
          }
        }
      }
    }

    return tiles;
  }

  void landscape::create_maze(world w, size_t width, size_t height) {
    auto r    = w.get_entity(w.get_root());
    auto land = std::make_shared<object::landscape>();

    set_size({ land, access::sync }, width, height);
    set_tiles({ land, access::sync }, gen_maze(w, width, height));
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

  auto landscape::get_tile( //
      entity en,            //
      size_t x,             //
      size_t y) -> int64_t {

    const auto tile = en.request( //
        sets::land_get_tile,      //
        pack_to_array(x, y));

    return rd<int64_t>(tile, 0);
  }

  auto landscape::get_tiles(entity en) -> vector<int64_t> {
    const auto bytes = en.request(sets::land_get_tiles);
    const auto count = bytes.size() / sizeof(int64_t);
    auto       tiles = vector<int64_t>(count);

    memcpy(tiles.data(), bytes.data(), count * sizeof(int64_t));
    return tiles;
  }

  void landscape::inc_version(entity en) {
    en.apply_delta(n_version, 1);
  }

  void landscape::set_tile( //
      entity  en,           //
      size_t  x,            //
      size_t  y,            //
      int64_t tile) {

    en.modify(               //
        sets::land_set_tile, //
        pack_to_array(x, y, tile));
  }

  void landscape::set_tiles(     //
      entity                 en, //
      const vector<int64_t> &tiles) {

    en.modify(sets::land_set_tiles,
              pack_to_bytes(span<const int64_t>(tiles)));
  }

  void landscape::set_size( //
      entity en,            //
      size_t width,         //
      size_t height) {

    en.modify(               //
        sets::land_set_size, //
        pack_to_array(width, height));
  }

  auto landscape::do_request(size_t id, cref_vbyte args) const
      -> vbyte {
    if (id == sets::land_get_tile) {

      return pack_to_bytes(get_tile( //
          rd<size_t>(args, args_x),  //
          rd<size_t>(args, args_y)));

    } else if (id == sets::land_get_tiles) {
      return pack_to_bytes(span<const int64_t>(m_tiles));
    }

    return {};
  }

  void landscape::do_modify(size_t id, cref_vbyte args) {
    if (id == sets::land_set_size) {

      set_size(                         //
          rd<size_t>(args, args_width), //
          rd<size_t>(args, args_height));

    } else if (id == sets::land_set_tile) {

      set_tile(                     //
          rd<size_t>(args, args_x), //
          rd<size_t>(args, args_y), //
          rd<int64_t>(args, args_tile));

    } else if (id == sets::land_set_tiles) {

      const auto size = min( //
          m_tiles.size(),    //
          args.size() / sizeof(int64_t));

      memcpy(             //
          m_tiles.data(), //
          args.data(),    //
          size * sizeof(int64_t));
    }
  }

  auto landscape::get_tile(size_t x, size_t y) const -> int64_t {
    const auto width  = static_cast<size_t>(locked_get(n_width));
    const auto height = static_cast<size_t>(locked_get(n_height));

    if (x >= width || y >= height)
      return 0;

    size_t index = y * width + x;

    if (index >= m_tiles.size())
      return 0;

    return m_tiles[index];
  }

  void landscape::set_tile(size_t x, size_t y, int64_t value) {
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
}

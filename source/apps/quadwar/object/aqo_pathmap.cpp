/*  apps/quadwar/object/aqo_pathmap.cpp
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

#include "pathmap.h"

#include "../../../laplace/core/serial.h"
#include "root.h"

namespace quadwar_app::object {
  namespace access = engine::access;

  using std::make_shared, std::span, engine::id_undefined;

  sl::index pathmap::n_width  = {};
  sl::index pathmap::n_height = {};

  pathmap pathmap::m_proto(pathmap::proto);

  pathmap::pathmap(proto_tag) {

    setup_sets({ { .id = sets::pathmap_width, .scale = 1 },
                 { .id = sets::pathmap_height, .scale = 1 } });

    n_width  = index_of(sets::pathmap_width);
    n_height = index_of(sets::pathmap_height);
  }

  pathmap::pathmap() : basic_entity(dummy) {
    *this = m_proto;
  }

  auto pathmap::create(world w) -> size_t {
    auto r  = w.get_entity(w.get_root());
    auto id = w.spawn(make_shared<pathmap>(), id_undefined);

    root::set_pathmap(r, id);
    r.adjust();

    return id;
  }

  void pathmap::set_tiles(entity en, const sl::whole width,
                          const sl::whole                height,
                          const std::span<const uint8_t> tiles) {

    if (width < 0 || height < 0)
      return;

    if (tiles.size() != width * height) {
      error_("Invalid tiles.", __FUNCTION__);
      return;
    }

    en.modify(sets::pathmap_set_tiles,
              serial::pack_to_bytes(width, height, tiles));
  }

  void pathmap::add(entity en, const sl::whole x, const sl::whole y,
                    const sl::whole width, const sl::whole height,
                    const span<const uint8_t> tiles) {

    if (x < 0 || y < 0 || width <= 0 || height <= 0)
      return;

    if (tiles.size() != width * height) {
      error_("Invalid tiles.", __FUNCTION__);
      return;
    }

    en.modify(sets::pathmap_add,
              serial::pack_to_bytes(x, y, width, height, tiles));
  }

  void pathmap::subtract(entity en, const sl::whole x,
                         const sl::whole y, const sl::whole width,
                         const sl::whole           height,
                         const span<const uint8_t> tiles) {

    if (x < 0 || y < 0 || width <= 0 || height <= 0)
      return;

    if (tiles.size() != width * height) {
      error_("Invalid tiles.", __FUNCTION__);
      return;
    }

    en.modify(sets::pathmap_subtract,
              serial::pack_to_bytes(x, y, width, height, tiles));
  }

  auto pathmap::get_width(entity en) -> sl::whole {
    return en.get(n_width);
  }

  auto pathmap::get_height(entity en) -> sl::whole {
    return en.get(n_height);
  }

  auto pathmap::get_tiles(entity en) -> sl::vector<uint8_t> {

    return en.request(sets::pathmap_get_tiles);
  }

  auto pathmap::check(entity en, const sl::whole x, const sl::whole y,
                      const sl::whole width, const sl::whole height,
                      const span<const uint8_t> tiles) -> bool {

    return serial::rd<uint8_t>(
               en.request(
                   sets::pathmap_check,
                   serial::pack_to_bytes(x, y, width, height, tiles)),
               0) != 0;
  }

  auto pathmap::do_request(size_t id, span_cbyte args) const -> vbyte {
    switch (id) {
      case sets::pathmap_get_tiles: return m_tiles;

      case sets::pathmap_check: {
        const auto x      = serial::rd<sl::whole>(args, args_x);
        const auto y      = serial::rd<sl::whole>(args, args_y);
        const auto width  = serial::rd<sl::whole>(args, args_width);
        const auto height = serial::rd<sl::whole>(args, args_height);

        const auto dst_width  = locked_get(n_width);
        const auto dst_height = locked_get(n_height);

        constexpr auto v_true = []() -> vbyte {
          auto v = vbyte(1);
          v[0]   = 1;
          return v;
        };

        constexpr auto v_false = []() -> vbyte {
          auto v = vbyte(1);
          v[0]   = 0;
          return v;
        };

        for (sl::index j = 0, k = 0; j < height; j++)
          for (sl::index i = 0; i < width; i++, k++) {
            const auto tile = serial::rd<uint8_t>(
                args, args_src_tiles + k);

            if (x + i < 0 || y + j < 0 || x + i >= dst_width ||
                y + j >= dst_height) {
              if (tile)
                return v_false();
              continue;
            }

            const auto n = (y + j) * dst_width + (x + i);

            if (n < 0 || n >= m_tiles.size()) {
              if (tile)
                return v_false();
              continue;
            }

            if (m_tiles[n] != 0 && tile != 0)
              return v_false();
          }

        return v_true();
      }

      default: error_("Invalid request id.", __FUNCTION__);
    }

    return {};
  }

  void pathmap::do_modify(size_t id, span_cbyte args) {
    switch (id) {
      case sets::pathmap_set_tiles: {
        const auto width  = serial::rd<sl::whole>(args, args_x);
        const auto height = serial::rd<sl::whole>(args, args_y);

        if (width < 0 || height < 0)
          return;

        m_tiles.resize(width * height);

        for (sl::index i = 0; i < m_tiles.size(); i++) {
          m_tiles[i] = serial::rd<uint8_t>(args, args_tiles + i);
        }

        init(n_width, width);
        init(n_height, height);

      } break;

      case sets::pathmap_add:
      case sets::pathmap_subtract: {
        const auto x      = serial::rd<sl::whole>(args, args_x);
        const auto y      = serial::rd<sl::whole>(args, args_y);
        const auto width  = serial::rd<sl::whole>(args, args_width);
        const auto height = serial::rd<sl::whole>(args, args_height);

        const auto dst_width  = locked_get(n_width);
        const auto dst_height = locked_get(n_height);

        for (sl::index j = 0, k = 0; j < height; j++)
          for (sl::index i = 0; i < width; i++, k++) {

            if (x + i < 0 || y + j < 0 || x + i >= dst_width ||
                y + j >= dst_height)
              continue;
            const auto n = (y + j) * dst_width + (x + i);
            if (n < 0 || n >= m_tiles.size())
              continue;

            const auto tile = serial::rd<uint8_t>(
                args, args_src_tiles + k);

            if (id == sets::pathmap_add) {
              m_tiles[n] += tile;
            } else {
              if (m_tiles[n] <= tile) {
                m_tiles[n] = 0;
              } else {
                m_tiles[n] -= tile;
              }
            }
          }

      } break;

      default: error_("Invalid request id.", __FUNCTION__);
    }
  }
}

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

  using std::make_shared, std::min, std::max, std::span,
      engine::id_undefined;

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

  auto pathmap::create(world w) -> sl::index {
    auto r  = w.get_entity(w.get_root());
    auto id = w.spawn(make_shared<pathmap>(), id_undefined);

    root::set_pathmap(r, id);
    r.adjust();

    return id;
  }

  void pathmap::set_tiles(entity en, const sl::whole width,
                          const sl::whole               height,
                          const std::span<const int8_t> tiles) {

    if (width < 0 || height < 0) {
      error_("Invalid size.", __FUNCTION__);
      return;
    }

    if (tiles.size() != width * height) {
      error_("Invalid tiles.", __FUNCTION__);
      return;
    }

    en.bytes_resize(tiles.size());

    for (sl::index i = 0; i < tiles.size(); i++) {
      en.bytes_set(i, tiles[i]);
    }

    en.set(n_width, width);
    en.set(n_height, height);

    en.adjust();
  }

  void pathmap::add(entity en, const sl::whole x0, const sl::whole y0,
                    const sl::whole width, const sl::whole height,
                    const span<const int8_t> tiles) {

    if (width <= 0 || height <= 0) {
      error_("Invalid size.", __FUNCTION__);
      return;
    }

    if (tiles.size() != width * height) {
      error_("Invalid tiles.", __FUNCTION__);
      return;
    }

    const auto dst_width  = en.get(n_width);
    const auto dst_height = en.get(n_height);

    for (sl::index j = 0; j < height; j++) {
      const auto y = y0 + j;

      if (y < 0 || y >= dst_height)
        continue;

      for (sl::index i = 0; i < width; i++) {
        const auto x = x0 + i;

        if (x < 0 || x >= dst_width)
          continue;

        const auto n = y * dst_width + x;

        en.bytes_apply_delta(n, tiles[j * width + i]);
      }
    }
  }

  void pathmap::subtract(entity en, const sl::whole x0,
                         const sl::whole y0, const sl::whole width,
                         const sl::whole          height,
                         const span<const int8_t> tiles) {

    if (width <= 0 || height <= 0) {
      error_("Invalid size.", __FUNCTION__);
      return;
    }

    if (tiles.size() != width * height) {
      error_("Invalid tiles.", __FUNCTION__);
      return;
    }

    const auto dst_width  = en.get(n_width);
    const auto dst_height = en.get(n_height);

    for (sl::index j = 0; j < height; j++) {
      const auto y = y0 + j;

      if (y < 0 || y >= dst_height)
        continue;

      for (sl::index i = 0; i < width; i++) {
        const auto x = x0 + i;

        if (x < 0 || x >= dst_width)
          continue;

        const auto n = y * dst_width + x;

        en.bytes_apply_delta(n, -tiles[j * width + i]);
      }
    }
  }

  auto pathmap::get_width(entity en) -> sl::whole {
    return en.get(n_width);
  }

  auto pathmap::get_height(entity en) -> sl::whole {
    return en.get(n_height);
  }

  auto pathmap::get_tiles(entity en) -> sl::vector<int8_t> {
    auto v = sl::vector<int8_t> {};

    v.resize(en.bytes_get_size());

    for (sl::index i = 0; i < v.size(); i++) {
      v[i] = en.bytes_get(i);
    }

    return v;
  }

  auto pathmap::check(entity en, const sl::whole x0, const sl::whole y0,
                      const sl::whole width, const sl::whole height,
                      const span<const int8_t> tiles) -> bool {

    if (width <= 0 || height <= 0) {
      error_("Invalid size.", __FUNCTION__);
      return false;
    }

    if (tiles.size() != width * height) {
      error_("Invalid tiles.", __FUNCTION__);
      return false;
    }

    const auto dst_width  = en.get(n_width);
    const auto dst_height = en.get(n_height);

    for (sl::index j = 0; j < height; j++) {
      const auto y = y0 + j;

      if (y < 0 || y >= dst_height)
        return false;

      for (sl::index i = 0; i < width; i++) {
        const auto x = x0 + i;

        if (x < 0 || x >= dst_width)
          return false;

        if (tiles[j * width + i] <= 0)
          continue;

        const auto n = y * dst_width + x;

        if (en.bytes_get(n) > 0)
          return false;
      }
    }

    return true;
  }
}

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

  using std::make_shared, std::min, std::max, std::span, std::array,
      engine::id_undefined, engine::vec2z;

  sl::index pathmap::n_width  = {};
  sl::index pathmap::n_height = {};

  pathmap pathmap::m_proto(pathmap::proto);

  const sl::vector<vec2z> pathmap::m_circular = pathmap::gen_circular();

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

  auto pathmap::place(entity en, const vec2z position, const vec2z size,
                      const span<const int8_t> footprint) noexcept
      -> vec2z {

    if (size.x() <= 0 || size.y() <= 0) {
      error_("Invalid size.", __FUNCTION__);
      return {};
    }

    if (size.x() * size.y() != footprint.size()) {
      error_("Invalid footprint.", __FUNCTION__);
      return {};
    }

    const auto width  = en.get(n_width);
    const auto height = en.get(n_height);

    auto line = sl::vector<int8_t>(size.x());

    for (sl::index k = 0; k < m_circular.size(); k++) {
      const auto x = position.x() + m_circular[k].x() * size.x();
      const auto y = position.y() + m_circular[k].y() * size.y();

      bool ok = true;

      for (sl::index j = 0; j < size.y(); j++) {
        if (y + j < 0 || y + j >= height)
          break;

        en.bytes_read((y + j) * width + x, line);

        for (sl::index i = 0; i < size.x(); i++) {
          if (x + i < 0 || x + i >= width) {
            ok = false;
            break;
          }

          if (footprint[j * size.x() + i] <= 0)
            continue;

          if (line[i] > 0) {
            ok = false;
            break;
          }
        }

        if (!ok)
          break;
      }

      if (ok) {
        for (sl::index j = 0; j < size.y(); j++) {
          en.bytes_write_delta(
              (y + j) * width + x,
              { footprint.begin() + (j * size.x()),
                footprint.begin() + ((j + 1) * size.x()) });
        }

        return { x, y };
      }
    }

    for (sl::index j = 0; j < size.y(); j++) {
      en.bytes_write_delta(
          (position.y() + j) * width + position.x(),
          { footprint.begin() + (j * size.x()),
            footprint.begin() + ((j + 1) * size.x()) });
    }

    return position;
  }

  auto pathmap::gen_circular() -> sl::vector<vec2z> {
    auto v = sl::vector<vec2z>(_circular_size);

    for (sl::index i = -_distance, k = 0; i <= _distance; i++) {
      for (sl::index j = -_distance; j <= _distance; j++, k++)
        v[k] = vec2z { i, j };
    }

    constexpr auto cmp = [](const vec2z a, const vec2z b) -> bool {
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
  }
}

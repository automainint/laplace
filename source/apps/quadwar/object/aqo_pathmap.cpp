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

  const sl::whole pathmap::resolution     = 2;
  const sl::whole pathmap::spawn_distance = 100;

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
    en.bytes_write(0, tiles);

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

    en.bytes_read(0, { v.begin(), v.end() });

    return v;
  }

  auto pathmap::check_move(entity en, const vec2z position,
                           const vec2z new_position, const vec2z size,
                           const span<const int8_t> footprint) noexcept
      -> bool {

    if (size.x() <= 0 || size.y() <= 0) {
      error_("Invalid size.", __FUNCTION__);
      return false;
    }

    if (size.x() * size.y() != footprint.size()) {
      error_("Invalid footprint.", __FUNCTION__);
      return false;
    }

    const auto width  = en.get(n_width);
    const auto height = en.get(n_height);

    const auto fp_center = size / sl::index(2);

    const auto x0 = new_position.x() - fp_center.x();
    const auto y0 = new_position.y() - fp_center.y();

    const auto x1 = new_position.x() + size.x() - fp_center.x();
    const auto y1 = new_position.y() + size.y() - fp_center.y();

    const auto dx = new_position.x() - position.x();
    const auto dy = new_position.y() - position.y();

    if (x0 < 0 || y0 < 0 || x1 >= width || y1 >= height) {
      return false;
    }

    auto line = sl::vector<int8_t>(size.x());

    for (sl::index j = 0; j < size.y(); j++) {
      en.bytes_read((y0 + j) * width + x0, line);

      for (sl::index i = 0; i < size.x(); i++) {
        const auto i0 = i + dx;
        const auto j0 = j + dy;

        if (i0 >= 0 && i0 < size.x() && j0 >= 0 && j0 < size.y() &&
            footprint[j0 * size.x() + i0] > 0) {
          continue;
        }

        if (footprint[j * size.x() + i] > 0 && line[i] > 0) {
          return false;
        }
      }
    }

    return true;
  }

  void pathmap::add(entity en, const vec2z position, const vec2z size,
                    const span<const int8_t> footprint) noexcept {

    if (size.x() <= 0 || size.y() <= 0) {
      error_("Invalid size.", __FUNCTION__);
      return;
    }

    if (size.x() * size.y() != footprint.size()) {
      error_("Invalid footprint.", __FUNCTION__);
      return;
    }

    const auto width  = en.get(n_width);
    const auto height = en.get(n_height);

    const auto fp_center = size / sl::index(2);

    const auto x0 = position.x() - fp_center.x();
    const auto y0 = position.y() - fp_center.y();

    const auto x1 = position.x() + size.x() - fp_center.x();
    const auto y1 = position.y() + size.y() - fp_center.y();

    if (x0 < 0 || y0 < 0 || x1 >= width || y1 >= height) {
      error_("Invalid position.", __FUNCTION__);
      return;
    }

    for (sl::index j = 0; j < size.y(); j++) {
      en.bytes_write_delta((y0 + j) * width + x0,
                           { footprint.begin() + (j * size.x()),
                             footprint.begin() + ((j + 1) * size.x()) });
    }
  }

  void pathmap::subtract(entity en, const engine::vec2z position,
                         const engine::vec2z           size,
                         const std::span<const int8_t> footprint) noexcept {

    if (size.x() <= 0 || size.y() <= 0) {
      error_("Invalid size.", __FUNCTION__);
      return;
    }

    if (size.x() * size.y() != footprint.size()) {
      error_("Invalid footprint.", __FUNCTION__);
      return;
    }

    const auto width  = en.get(n_width);
    const auto height = en.get(n_height);

    const auto fp_center = size / sl::index(2);

    const auto x0 = position.x() - fp_center.x();
    const auto y0 = position.y() - fp_center.y();

    const auto x1 = position.x() + size.x() - fp_center.x();
    const auto y1 = position.y() + size.y() - fp_center.y();

    if (x0 < 0 || y0 < 0 || x1 >= width || y1 >= height) {
      error_("Invalid position.", __FUNCTION__);
      return;
    }

    for (sl::index j = 0; j < size.y(); j++) {
      en.bytes_erase_delta((y0 + j) * width + x0,
                           { footprint.begin() + (j * size.x()),
                             footprint.begin() + ((j + 1) * size.x()) });
    }
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

    const auto delta = vec2z { spawn_distance, spawn_distance };

    const auto area = adjust_rect(position - delta, position + delta,
                                  { width, height });

    const auto area_size = area.max - area.min;

    const auto area_src = [&en, &width, &area,
                           &area_size]() -> sl::vector<int8_t> {
      auto v = sl::vector<int8_t>(area_size.x() * area_size.y());

      for (sl::index j = 0; j < area_size.y(); j++) {
        const auto dst = v.begin() + (j * area_size.x());

        en.bytes_read((area.min.y() + j) * width + area.min.x(),
                      { dst, dst + area_size.x() });
      }

      return v;
    }();

    const auto fp_center = size / sl::index(2);

    const auto area_dst = [&size, &fp_center, &footprint, &area,
                           &area_size,
                           &area_src]() -> sl::vector<int8_t> {
      auto v = sl::vector<int8_t>(area_src.size());

      convolve(fp_center, size, footprint, area_size, area_src, v);

      return v;
    }();

    const auto p = area.min +
                   nearest(position - area.min, area_size, area_dst);

    const auto p0 = p - fp_center;

    const auto i0 = -max<sl::index>(0, -p0.x());
    const auto i1 = min(width, p0.x() + size.x()) - p0.x();

    const auto j0 = -max<sl::index>(0, -p0.y());
    const auto j1 = min(height, p0.y() + size.y()) - p0.y();

    for (sl::index j = j0; j < j1; j++) {
      const auto src = footprint.begin() + (j * size.x());

      en.bytes_write_delta((p0.y() + j) * width + p0.x() + i0,
                           { src + i0, src + i1 });
    }

    return p;
  }

  auto pathmap::adjust_rect(const vec2z min, const vec2z max,
                            const vec2z bounds) noexcept
      -> adjust_rect_result {

    if (bounds.x() < 0 || bounds.y() < 0) {
      error_("Invalid bounds.", __FUNCTION__);
      return {};
    }

    if (min.x() > max.x() || min.y() > max.y()) {
      error_("Invalid rect.", __FUNCTION__);
      return {};
    }

    auto res = adjust_rect_result { .min = min, .max = max };

    if (min.x() < 0)
      res.min.x() = 0;
    if (min.y() < 0)
      res.min.y() = 0;

    if (min.x() > bounds.x())
      res.min.x() = bounds.x();
    if (min.y() > bounds.y())
      res.min.y() = bounds.y();

    if (res.max.x() > bounds.x())
      res.max.x() = bounds.x();
    if (res.max.y() > bounds.y())
      res.max.y() = bounds.y();

    return res;
  }

  void pathmap::convolve(const vec2z center, const vec2z fp_size,
                         const span<const int8_t> footprint,
                         const vec2z size, const span<const int8_t> src,
                         const span<int8_t> dst) noexcept {

    if (fp_size.x() < 0 || fp_size.y() < 0) {
      error_("Invalid footprint size.", __FUNCTION__);
      return;
    }

    if (size.x() < 0 || size.y() < 0) {
      error_("Invalid size.", __FUNCTION__);
      return;
    }

    if (fp_size.x() * fp_size.y() != footprint.size()) {
      error_("Invalid footprint.", __FUNCTION__);
      return;
    }

    if (size.x() * size.y() != src.size()) {
      error_("Invalid source.", __FUNCTION__);
      return;
    }

    if (src.size() != dst.size()) {
      error_("Invalid destination.", __FUNCTION__);
      return;
    }

    for (sl::index j0 = 0; j0 < size.y(); j0++)
      for (sl::index i0 = 0; i0 < size.x(); i0++) {

        if (src[j0 * size.x() + i0] <= 0)
          continue;

        for (sl::index n = 0; n < fp_size.y(); n++)
          for (sl::index m = 0; m < fp_size.x(); m++) {

            const auto x = footprint[n * fp_size.x() + m];

            if (x <= 0)
              continue;

            const auto i = i0 + m - center.x();
            const auto j = j0 + n - center.y();

            if (i < 0 || i >= size.x() || j < 0 || j >= size.y())
              continue;

            dst[j * size.x() + i] += x;
          }
      }
  }

  auto pathmap::nearest(const vec2z center, const vec2z size,
                        const span<const int8_t> map) noexcept
      -> vec2z {

    if (size.x() < 0 || size.y() < 0) {
      error_("Invalid size.", __FUNCTION__);
      return {};
    }

    if (size.x() * size.y() != map.size()) {
      error_("Invalid map.", __FUNCTION__);
      return {};
    }

    if (center.x() >= 0 && center.x() < size.x() && center.y() >= 0 &&
        center.y() < size.y() &&
        map[center.y() * size.x() + center.x()] <= 0)
      return center;

    auto      p        = center;
    sl::index distance = -1;

    for (sl::index j = 0, k = 0; j < size.y(); j++)
      for (sl::index i = 0; i < size.x(); i++, k++) {

        if (map[k] > 0)
          continue;

        const auto v = vec2z { i, j };
        const auto d = math::square_length(center - v);

        if (distance < 0 || d < distance) {
          p        = v;
          distance = d;
        }
      }

    return p;
  }
}

/*  apps/quadwar/action/aqa_pathmap_reset.cpp
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

#include "pathmap_reset.h"

#include "../object/landscape.h"
#include "../object/pathmap.h"
#include "../object/root.h"

namespace quadwar_app::action {
  namespace access = engine::access;

  using object::root, object::landscape, object::pathmap;

  pathmap_reset::pathmap_reset(size_t id_pathmap) {
    m_id_pathmap = id_pathmap;
  }

  void pathmap_reset::perform(access::world w) const {
    auto r    = w.get_entity(w.get_root());
    auto land = w.get_entity(root::get_landscape(r));
    auto path = w.get_entity(m_id_pathmap);

    const auto width      = landscape::get_width(land);
    const auto height     = landscape::get_height(land);
    const auto dst_width  = width * pathmap::resolution;
    const auto dst_height = height * pathmap::resolution;

    const auto src_tiles = landscape::get_tiles(land);
    auto       dst_tiles = vbyte(dst_width * dst_height);

    for (sl::index j = 0; j < height; j++)
      for (sl::index i = 0; i < width; i++) {
        auto x0 = i * pathmap::resolution;
        auto y0 = j * pathmap::resolution;
        auto x1 = (i + 1) * pathmap::resolution;
        auto y1 = (j + 1) * pathmap::resolution;

        for (auto y = y0; y < y1; y++) {
          for (auto x = x0; x < x1; x++)
            dst_tiles[y * dst_width + x] = src_tiles[j * width + i];
        }
      }

    pathmap::set_tiles(path, dst_width, dst_height, dst_tiles);
  }
}

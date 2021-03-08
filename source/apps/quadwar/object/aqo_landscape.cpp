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

namespace quadwar_app::object {
  using engine::basic_entity, std::vector, std::span, std::min;

  size_t landscape::n_width  = 0;
  size_t landscape::n_height = 0;

  landscape landscape::m_proto(landscape::proto);

  landscape::landscape(proto_tag) {

    setup_sets({ { .id = sets::land_width, .scale = 1 },
                 { .id = sets::land_height, .scale = 1 } });

    n_width  = index_of(sets::land_width);
    n_height = index_of(sets::land_height);
  }

  landscape::landscape() : basic_entity(dummy) {
    *this = m_proto;
  }

  void landscape::generate(entity en, size_t width, size_t height) {
    set_size(en, width, height);
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

    init(n_width, width);
    init(n_height, height);
  }
}

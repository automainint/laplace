/*  apps/quadwar/object/landscape.h
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

#ifndef quadwar_object_landscape_h
#define quadwar_object_landscape_h

#include "../../../laplace/engine/basic_entity.h"
#include "defs.h"

namespace quadwar_app::object {
  class landscape : public engine::basic_entity, helper {
  public:
    enum tile_state : int8_t { tile_walkable, tile_wall };

    static constexpr sl::index cave_size   = 5;
    static constexpr sl::index tunnel_size = 1;
    static constexpr sl::index gate_size   = 2;

    landscape();
    ~landscape() override = default;

    static auto gen_maze(world w, sl::index width, sl::index height,
                         std::span<const engine::vec2z> start_locs) noexcept
        -> std::vector<int8_t>;

    static auto gen_start_locs(sl::whole width, sl::whole height,
                               sl::whole edge,
                               sl::whole player_count) noexcept
        -> std::vector<engine::vec2z>;

    static void create_maze(world w, sl::index width,
                            sl::index height, sl::index player_count);

    [[nodiscard]] static auto get_version(entity en) -> sl::index;
    [[nodiscard]] static auto get_width(entity en) -> sl::index;
    [[nodiscard]] static auto get_height(entity en) -> sl::index;

    [[nodiscard]] static auto get_tile(entity en, sl::index x,
                                       sl::index y) -> int8_t;

    [[nodiscard]] static auto get_start_loc(entity en, sl::index index)
        -> engine::vec2z;

    [[nodiscard]] static auto get_tiles(entity en)
        -> sl::vector<int8_t>;

    [[nodiscard]] static auto get_start_locs(entity en)
        -> sl::vector<engine::vec2z>;

    static void inc_version(entity en);

    static void set_tile(entity en, sl::index x, sl::index y,
                         int8_t tile);

    static void set_start_loc(entity en, sl::index n,
                              engine::vec2z start_loc);

    static void set_tiles(entity en, std::span<const int8_t> tiles);

    static void set_size(entity en, sl::whole width, sl::whole height);

    static void set_start_locs(entity                         en,
                               std::span<const engine::vec2z> locs);

  private:
    landscape(proto_tag);

    static sl::index n_version;
    static sl::index n_width;
    static sl::index n_height;

    static landscape m_proto;
  };
}

#endif

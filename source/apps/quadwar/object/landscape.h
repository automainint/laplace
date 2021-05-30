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

    static void set_tiles(entity en, const sl::vector<int8_t> &tiles);

    static void set_size(entity en, sl::whole width, sl::whole height);

    static void set_start_locs(entity                         en,
                               std::span<const engine::vec2z> locs);

  protected:
    auto do_request(sl::index id, span_cbyte args) const
        -> vbyte override;
    void do_modify(sl::index id, span_cbyte args) override;

  private:
    enum args_offset : sl::index {
      args_x           = 0,
      args_y           = args_x + sizeof(sl::index),
      args_tile        = args_y + sizeof(sl::index),
      args_width       = 0,
      args_height      = args_width + sizeof(sl::index),
      args_start_loc   = 0,
      args_start_loc_x = args_start_loc + sizeof(sl::index),
      args_start_loc_y = args_start_loc_x + sizeof(sl::index)
    };

    landscape(proto_tag);

    [[nodiscard]] auto get_tile(sl::index x, sl::index y) const
        -> int8_t;
    [[nodiscard]] auto get_start_loc(sl::index index) const
        -> engine::vec2z;

    void set_tile(sl::index x, sl::index y, int8_t value);
    void set_size(sl::index width, sl::index height);
    void set_start_loc(sl::index index, const engine::vec2z loc);

    static sl::index n_version;
    static sl::index n_width;
    static sl::index n_height;

    static landscape m_proto;

    sl::vector<int8_t>        m_tiles;
    sl::vector<engine::vec2z> m_start_locs;
  };
}

#endif

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
    enum tile_state : uint8_t { tile_walkable, tile_wall };

    static constexpr size_t cave_size   = 5;
    static constexpr size_t tunnel_size = 1;
    static constexpr size_t gate_size   = 2;

    landscape();
    ~landscape() override = default;

    static auto gen_maze(world w, size_t width, size_t height,
                         std::span<const engine::vec2z> start_locs) noexcept
        -> std::vector<uint8_t>;

    static auto gen_start_locs(sl::whole width, sl::whole height,
                               sl::whole edge,
                               sl::whole player_count) noexcept
        -> std::vector<engine::vec2z>;

    static void create_maze(world w, size_t width, size_t height,
                            size_t player_count);

    [[nodiscard]] static auto get_version(entity en) -> size_t;
    [[nodiscard]] static auto get_width(entity en) -> size_t;
    [[nodiscard]] static auto get_height(entity en) -> size_t;

    [[nodiscard]] static auto get_tile(entity en, size_t x, size_t y)
        -> uint8_t;

    [[nodiscard]] static auto get_start_loc(entity en, size_t index)
        -> engine::vec2z;

    [[nodiscard]] static auto get_tiles(entity en)
        -> std::vector<uint8_t>;

    [[nodiscard]] static auto get_start_locs(entity en)
        -> std::vector<engine::vec2z>;

    static void inc_version(entity en);

    static void set_tile(entity en, size_t x, size_t y, uint8_t tile);

    static void set_start_loc(entity en, size_t index,
                              engine::vec2z start_loc);

    static void set_tiles(entity en, const std::vector<uint8_t> &tiles);

    static void set_size(entity en, size_t width, size_t height);

    static void set_start_locs(entity                         en,
                               std::span<const engine::vec2z> locs);

  protected:
    auto do_request(size_t id, span_cbyte args) const -> vbyte override;
    void do_modify(size_t id, span_cbyte args) override;

  private:
    enum args_offset : size_t {
      args_x           = 0,
      args_y           = args_x + sizeof(size_t),
      args_tile        = args_y + sizeof(size_t),
      args_width       = 0,
      args_height      = args_width + sizeof(size_t),
      args_start_loc   = 0,
      args_start_loc_x = args_start_loc + sizeof(size_t),
      args_start_loc_y = args_start_loc_x + sizeof(size_t)
    };

    landscape(proto_tag);

    [[nodiscard]] auto get_tile(size_t x, size_t y) const -> uint8_t;
    [[nodiscard]] auto get_start_loc(size_t index) const
        -> engine::vec2z;

    void set_tile(size_t x, size_t y, uint8_t value);
    void set_size(size_t width, size_t height);
    void set_start_loc(size_t index, const engine::vec2z loc);

    static size_t n_version;
    static size_t n_width;
    static size_t n_height;

    static landscape m_proto;

    std::vector<uint8_t>       m_tiles;
    std::vector<engine::vec2z> m_start_locs;
  };
}

#endif

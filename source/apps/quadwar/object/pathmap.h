/*  apps/quadwar/object/pathmap.h
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

#ifndef quadwar_object_pathmap_h
#define quadwar_object_pathmap_h

#include "../../../laplace/engine/basic_entity.h"
#include "defs.h"

namespace quadwar_app::object {
  class pathmap : public engine::basic_entity, helper {
  public:
    static const sl::whole resolution;
    static const sl::whole spawn_distance;
    static const sl::whole search_scale;

    pathmap();
    ~pathmap() override = default;

    static auto create(world w) -> sl::index;

    static void set_tiles(entity en, const sl::whole width,
                          const sl::whole               height,
                          const std::span<const int8_t> tiles);

    [[nodiscard]] static auto get_width(entity en) -> sl::whole;
    [[nodiscard]] static auto get_height(entity en) -> sl::whole;
    [[nodiscard]] static auto get_tiles(entity en)
        -> sl::vector<int8_t>;

    [[nodiscard]] static auto check_move(
        entity en, const engine::vec2z position,
        const engine::vec2z new_position, const engine::vec2z size,
        const std::span<const int8_t> footprint) noexcept -> bool;

    static void add(entity en, const engine::vec2z position,
                    const engine::vec2z           size,
                    const std::span<const int8_t> footprint) noexcept;

    static void subtract(entity en, const engine::vec2z position,
                         const engine::vec2z           size,
                         const std::span<const int8_t> footprint) noexcept;

    [[nodiscard]] static auto find_empty(
        entity en, const engine::vec2z position,
        const engine::vec2z           size,
        const std::span<const int8_t> footprint) noexcept
        -> engine::vec2z;

    [[nodiscard]] static auto path_search(
        entity en, const engine::vec2z origin,
        const engine::vec2z target, const engine::vec2z fp_size,
        const std::span<const int8_t> footprint) noexcept
        -> sl::vector<engine::vec2z>;

  private:
    pathmap(proto_tag);

    struct adjust_rect_result {
      engine::vec2z min = {};
      engine::vec2z max = {};
    };

    [[nodiscard]] static auto adjust_rect(
        const engine::vec2z min, const engine::vec2z max,
        const engine::vec2z bounds) noexcept -> adjust_rect_result;

    static void convolve(const engine::vec2z           center,
                         const engine::vec2z           fp_size,
                         const std::span<const int8_t> footprint,
                         const engine::vec2z           size,
                         const std::span<const int8_t> src,
                         const std::span<int8_t>       dst) noexcept;

    [[nodiscard]] static auto nearest(
        const engine::vec2z center, const engine::vec2z size,
        const std::span<const int8_t> map) noexcept -> engine::vec2z;

    static sl::index n_width;
    static sl::index n_height;

    static pathmap m_proto;
  };
}

#endif

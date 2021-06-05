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
    static constexpr sl::whole resolution     = 2;
    static constexpr sl::whole _distance      = 20;
    static constexpr sl::whole _circular_size = (_distance * 2 + 1) *
                                                (_distance * 2 + 1);

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

    [[nodiscard]] static auto place(
        entity en, const engine::vec2z position,
        const engine::vec2z           size,
        const std::span<const int8_t> footprint) noexcept
        -> engine::vec2z;

    static auto gen_circular()
        -> sl::vector<engine::vec2z>;

  private:
    pathmap(proto_tag);

    static sl::index n_width;
    static sl::index n_height;

    static pathmap m_proto;

    static const sl::vector<engine::vec2z> m_circular;
  };
}

#endif

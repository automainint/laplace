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
    static constexpr sl::whole resolution = 10;

    pathmap();
    ~pathmap() override = default;

    static auto create(world w) -> sl::index;

    static void set_tiles(entity en, const sl::whole width,
                          const sl::whole               height,
                          const std::span<const int8_t> tiles);

    static void add(entity en, const sl::whole x0, const sl::whole y0,
                    const sl::whole width, const sl::whole height,
                    const std::span<const int8_t> tiles);

    static void subtract(entity en, const sl::whole x0,
                         const sl::whole y0, const sl::whole width,
                         const sl::whole               height,
                         const std::span<const int8_t> tiles);

    [[nodiscard]] static auto get_width(entity en) -> sl::whole;
    [[nodiscard]] static auto get_height(entity en) -> sl::whole;
    [[nodiscard]] static auto get_tiles(entity en)
        -> sl::vector<int8_t>;

    [[nodiscard]] static auto check(entity en, const sl::whole x0,
                                    const sl::whole y0,
                                    const sl::whole width,
                                    const sl::whole height,
                                    const std::span<const int8_t> tiles)
        -> bool;

  private:
    pathmap(proto_tag);

    static sl::index n_width;
    static sl::index n_height;

    static pathmap m_proto;
  };
}

#endif

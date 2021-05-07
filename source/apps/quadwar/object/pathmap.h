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

    static auto create(world w) -> size_t;

    static void set_tiles(entity en, const sl::whole width,
                          const sl::whole                height,
                          const std::span<const uint8_t> tiles);

    static void add(entity en, const sl::whole x, const sl::whole y,
                    const sl::whole width, const sl::whole height,
                    const std::span<const uint8_t> tiles);

    static void subtract(entity en, const sl::whole x,
                         const sl::whole y, const sl::whole width,
                         const sl::whole                height,
                         const std::span<const uint8_t> tiles);

    [[nodiscard]] static auto get_width(entity en) -> sl::whole;
    [[nodiscard]] static auto get_height(entity en) -> sl::whole;
    [[nodiscard]] static auto get_tiles(entity en)
        -> sl::vector<uint8_t>;

    [[nodiscard]] static auto check(
        entity en, const sl::whole x, const sl::whole y,
        const sl::whole width, const sl::whole height,
        const std::span<const uint8_t> tiles) -> bool;

  protected:
    auto do_request(size_t id, span_cbyte args) const -> vbyte override;
    void do_modify(size_t id, span_cbyte args) override;

  private:
    enum args_offset : sl::index {
      args_x         = 0,
      args_y         = args_x + sizeof(sl::whole),
      args_width     = args_y + sizeof(sl::whole),
      args_height    = args_width + sizeof(sl::whole),
      args_tiles     = args_y + sizeof(sl::whole),
      args_src_tiles = args_height + sizeof(sl::whole)
    };

    pathmap(proto_tag);

    static sl::index n_width;
    static sl::index n_height;

    static pathmap m_proto;

    std::vector<uint8_t> m_tiles;
  };
}

#endif

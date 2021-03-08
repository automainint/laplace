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
    landscape();

    static void generate(entity en, size_t width, size_t height);

    [[nodiscard]] static auto get_width(entity en) -> size_t;
    [[nodiscard]] static auto get_height(entity en) -> size_t;

    [[nodiscard]] static auto get_tile( //
        entity en,                      //
        size_t x,                       //
        size_t y) -> int64_t;

    [[nodiscard]] static auto get_tiles(entity en)
        -> std::vector<int64_t>;

    static void set_tile( //
        entity  en,       //
        size_t  x,        //
        size_t  y,        //
        int64_t tile);

    static void set_tiles(              //
        entity                      en, //
        const std::vector<int64_t> &tiles);

    static void set_size( //
        entity en,        //
        size_t width,     //
        size_t height);

  protected:
    auto do_request(size_t id, cref_vbyte args) const -> vbyte override;
    void do_modify(size_t id, cref_vbyte args) override;

  private:
    enum args_offset : size_t { //
      args_x      = 0,          //
      args_y      = 8,          //
      args_tile   = 16,         //
      args_width  = 0,          //
      args_height = 8,
    };

    landscape(proto_tag);

    [[nodiscard]] auto get_tile(size_t x, size_t y) const -> int64_t;

    void set_tile(size_t x, size_t y, int64_t value);
    void set_size(size_t width, size_t height);

    static size_t n_width;
    static size_t n_height;

    static landscape m_proto;

    std::vector<int64_t> m_tiles;
  };
}

#endif

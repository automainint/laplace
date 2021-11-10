/*  Copyright (c) 2021 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef quadwar_object_player_h
#define quadwar_object_player_h

#include "../../../laplace/engine/object/actor.h"
#include "defs.h"

namespace quadwar_app::object {
  class player : public actor, helper {
  public:
    explicit player(bool is_local = false) noexcept;
    ~player() noexcept override = default;

    static void init_indices(world w);
    static void set_name(entity en, std::u8string_view name);
    static void set_index(entity en, sl::index n);
    static auto get_name(entity en) -> std::u8string;
    static auto get_index(entity en) -> sl::index;

  private:
    explicit player(proto_tag) noexcept;

    static sl::index n_player_index;

    static player m_proto;
  };
}

#endif

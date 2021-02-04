/*  apps/quadwar/player.h
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

#ifndef __quadwar__player__
#define __quadwar__player__

#include "../../laplace/engine/access/entity.h"
#include "../../laplace/engine/object/actor.h"

namespace quadwar_app {
  using namespace laplace;

  class player : public engine::object::actor {
  public:
    player(bool is_local = false);
    ~player() override = default;

    static void set_name(engine::access::entity en,
                         std::u8string_view     name);
    static auto get_name(engine::access::entity en)
        -> std::u8string;

  protected:
    auto do_request(size_t id, cref_vbyte args) const
        -> vbyte override;
    void do_modify(size_t id, cref_vbyte args) override;

  private:
    player(proto_tag);

    static player m_proto;

    std::u8string m_name;
  };
}

#endif

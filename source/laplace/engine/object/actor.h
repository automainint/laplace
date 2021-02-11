/*  laplace/engine/object/actor.h
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

#ifndef __laplace__engine_object_actor__
#define __laplace__engine_object_actor__

#include "../access/entity.h"
#include "../basic_entity.h"

namespace laplace::engine::object {
  class actor : public basic_entity {
  public:
    ~actor() override = default;

    static void set_local(access::entity en, bool is_local);
    static auto is_local(access::entity en) -> bool;

  protected:
    actor(dummy_tag);
    actor(proto_tag);

    static size_t n_is_local;

  private:
    static actor m_proto;
  };
}

#endif

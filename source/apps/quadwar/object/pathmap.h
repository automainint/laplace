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
    pathmap();
    ~pathmap() override = default;

    void tick(engine::access::world w) override;

  private:
    pathmap(proto_tag);

    static pathmap m_proto;
  };
}

#endif

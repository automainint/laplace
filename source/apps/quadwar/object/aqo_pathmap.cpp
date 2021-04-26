/*  apps/quadwar/object/aqo_pathmap.cpp
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

#include "../action/pathmap_reset.h"
#include "pathmap.h"

namespace quadwar_app::object {
  namespace access = engine::access;

  using std::make_shared, action::pathmap_reset;

  pathmap pathmap::m_proto(pathmap::proto);

  pathmap::pathmap(proto_tag) : basic_entity(1) { }

  pathmap::pathmap() : basic_entity(dummy) {
    *this = m_proto;
  }

  void pathmap::tick(access::world w) {
    w.queue(make_shared<pathmap_reset>(get_id()));
  }
}

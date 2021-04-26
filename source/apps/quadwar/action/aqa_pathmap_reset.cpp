/*  apps/quadwar/action/aqa_pathmap_reset.cpp
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

#include "pathmap_reset.h"

namespace quadwar_app::action {
  namespace access = engine::access;

  pathmap_reset::pathmap_reset(size_t id_pathmap) {
    m_id_pathmap = id_pathmap;
  }

  void pathmap_reset::perform(access::world w) const {
  }
}

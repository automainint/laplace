/*  laplace/render/r_mesh.cpp
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

#include "mesh.h"
#include "model.h"

namespace laplace::render {
  void mesh::setup(ptr_model source) {
    error_("Not implemented.", __FUNCTION__);
  }

  void mesh::adjust(cref_motion state) {
    error_("Not implemented.", __FUNCTION__);
  }
}

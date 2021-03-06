/*  laplace/render/r_model.cpp
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

#include "model.h"

namespace laplace::render {
  model::model() {
    static element::type type_model;
    set_type_id(type_model.get_id());
  }

  void model::clear() {
    nodes.clear();
    bunches.clear();
    lights.clear();
  }
}

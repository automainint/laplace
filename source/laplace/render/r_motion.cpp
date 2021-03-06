/*  laplace/render/r_motion.cpp
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
#include "motion.h"

namespace laplace::render {
  void motion::setup(ptr_model source) { }
  void motion::solve(timeval time) { }
  void motion::blend(cref_motion state, realmax_t factor) { }
}

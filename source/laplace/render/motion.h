/*  laplace/render/motion.h
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

#ifndef laplace_render_motion_h
#define laplace_render_motion_h

#include "defs.h"
#include <memory>
#include <vector>

namespace laplace::render {
  class model;

  using ptr_model = std::shared_ptr<model>;

  class motion {
  public:
    using cref_motion = const motion &;

    void setup(ptr_model source);
    void solve(timeval time);
    void blend(cref_motion state, realmax_t factor);

  private:
    ptr_model m_source;
  };

  using cref_motion = motion::cref_motion;
  using ref_motion  = motion &;
  using ptr_motion  = std::shared_ptr<motion>;
}

#endif

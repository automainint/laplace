/*  laplace/render/mesh.h
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

#ifndef laplace_render_mesh_h
#define laplace_render_mesh_h

#include "../graphics/tridi/mesh_buffer.h"
#include "motion.h"
#include <memory>
#include <vector>

namespace laplace::render {
  class model;

  using ptr_model = std::shared_ptr<model>;

  class mesh {
  public:
    void setup(ptr_model source);
    void adjust(cref_motion state);

  private:
    ptr_model m_source;
  };

  using cref_mesh = const mesh &;
  using ptr_mesh  = std::shared_ptr<mesh>;
  using vptr_mesh = std::vector<ptr_mesh>;
}

#endif

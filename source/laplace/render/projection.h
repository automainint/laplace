/*  laplace/render/projection.h
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

#ifndef laplace_render_projection_h
#define laplace_render_projection_h

#include "defs.h"
#include <variant>

namespace laplace::render {
  class projection {
  public:
    struct ortho {
      real left   = 0.f;
      real right  = 0.f;
      real bottom = 0.f;
      real top    = 0.f;
      real near   = 0.f;
      real far    = 0.f;
    };

    struct perspective {
      real fovy   = 0.f;
      real aspect = 0.f;
      real near   = 0.f;
      real far    = 0.f;
    };

    mat4 matrix = unit<mat4>;

    void set_matrix(cref_mat4 matrix);

    void set_ortho(            //
        real left, real right, //
        real bottom, real top, //
        real near, real far);

    void set_perspective( //
        real fovy,        //
        real aspect,      //
        real near, real far);

    auto is_matrix() const -> bool;
    auto is_ortho() const -> bool;
    auto is_perspective() const -> bool;

    auto get_ortho() const -> ortho;
    auto get_perspective() const -> perspective;

  private:
    enum { n_matrix = 0, n_ortho, n_perspective };

    std::variant<std::monostate, ortho, perspective> m_data;
  };

  using cref_projection = const projection &;
}

#endif

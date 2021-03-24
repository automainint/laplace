/*  laplace/render/r_projection.cpp
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

#include "../core/defs.h"
#include "../math/transform.h"
#include "projection.h"

namespace laplace::render {
  using std::monostate;

  void projection::set_matrix(cref_mat4 mat) {
    this->m_data = monostate();
    this->matrix = mat;
  }

  void projection::set_ortho(real left, real right, real bottom,
                             real top, real near, real far) {

    m_data = ortho { left, right, bottom, top, near, far };

    error(__FUNCTION__, "Not implemented.");
  }

  void projection::set_perspective(real fovy, real aspect, real near,
                                   real far) {

    m_data = perspective { fovy, aspect, near, far };

    error(__FUNCTION__, "Not implemented.");
  }

  auto projection::is_matrix() const -> bool {
    return m_data.index() == n_matrix;
  }

  auto projection::is_ortho() const -> bool {
    return m_data.index() == n_ortho;
  }

  auto projection::is_perspective() const -> bool {
    return m_data.index() == n_perspective;
  }

  auto projection::get_ortho() const -> projection::ortho {

    if (m_data.index() != n_ortho) {
      error(__FUNCTION__, "Not ortho projection.");
      return {};
    }

    return get<n_ortho>(m_data);
  }

  auto projection::get_perspective() const -> projection::perspective {

    if (m_data.index() != n_perspective) {
      error(__FUNCTION__, "Not perspective projection.");
      return {};
    }

    return get<n_perspective>(m_data);
  }
}

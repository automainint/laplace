/*  laplace/math/shortnames.h
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

#ifndef __laplace__math_shortnames__
#define __laplace__math_shortnames__

#include <cstdint>

namespace laplace::math {
  template <size_t size_, typename type_>
  class vector;

  template <size_t rows_, size_t cols_, typename type_>
  class matrix;

  template <typename type_>
  class complex;

  template <typename type_>
  class quaternion;

  namespace shortnames {
    using real = double;

    using vec2 = vector<2, real>;
    using vec3 = vector<3, real>;
    using vec4 = vector<4, real>;

    using mat2 = matrix<2, 2, real>;
    using mat3 = matrix<3, 3, real>;
    using mat4 = matrix<4, 4, real>;

    using comp = complex<real>;
    using quat = quaternion<real>;
  }
}

#endif

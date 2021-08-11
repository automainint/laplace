/*  laplace/math/defs.h
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

#ifndef laplace_math_defs_h
#define laplace_math_defs_h

#include "../core/options.h"
#include "../core/slib.h"
#include <cstddef>
#include <cstdint>

namespace laplace::math {
  using realmax_t = long double;

  template <sl::whole size_, typename type_>
  class vector;

  template <sl::whole rows_, sl::whole cols_, typename type_>
  class matrix;

  template <typename type_>
  class complex;

  template <typename type_>
  class quaternion;

  template <typename type_>
  struct type_helper {
    using scalar_type = type_;
    using elem_type   = type_;

    static constexpr bool is_scalar = true;
  };

  template <sl::whole size_, typename type_>
  struct type_helper<vector<size_, type_>> {
    using scalar_type = typename type_helper<type_>::scalar_type;
    using elem_type   = type_;

    static constexpr bool is_scalar = false;
  };

  template <sl::whole rows_, sl::whole cols_, typename type_>
  struct type_helper<matrix<rows_, cols_, type_>> {
    using scalar_type = typename type_helper<type_>::scalar_type;
    using elem_type   = type_;

    static constexpr bool is_scalar = false;
  };

  template <typename type_>
  struct type_helper<complex<type_>> {
    using scalar_type = typename type_helper<type_>::scalar_type;
    using elem_type   = type_;

    static constexpr bool is_scalar = false;
  };

  template <typename type_>
  struct type_helper<quaternion<type_>> {
    using scalar_type = typename type_helper<type_>::scalar_type;
    using elem_type   = type_;

    static constexpr bool is_scalar = false;
  };

  template <typename type_>
  using scalar_type = typename type_helper<type_>::scalar_type;

  template <typename type_>
  using elem_type = typename type_helper<type_>::elem_type;

  template <typename type_>
  constexpr auto is_scalar = type_helper<type_>::is_scalar;
}

#endif

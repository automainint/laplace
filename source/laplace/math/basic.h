/*  laplace/math/basic.h
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

#ifndef laplace_math_basic_h
#define laplace_math_basic_h

#include "traits.h"
#include <cmath>
#include <concepts>
#include <functional>

namespace laplace::math {
  template <typename type_>
  constexpr auto equals(type_ a, type_ b) noexcept -> bool;

  template <typename type_>
  constexpr auto equals(type_              a,
                        type_              b,
                        scalar_type<type_> epsilon) noexcept -> bool;

  template <typename type_, typename arg_>
  inline auto round(arg_ t) noexcept -> type_;

  template <typename type_, typename sqrt_>
  auto length(type_ v, sqrt_ sqrt) -> scalar_type<type_>;

  template <typename type_>
  auto length(type_ v) -> scalar_type<type_>;

  /*  Linear interpolation.
   */
  template <typename type_>
  constexpr auto lerp(type_ begin, type_ end, realmax_t t) noexcept
      -> type_;
}

#include "basic.impl.h"

#endif

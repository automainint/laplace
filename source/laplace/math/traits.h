/*  laplace/math/traits.h
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

#ifndef laplace_math_traits_h
#define laplace_math_traits_h

#include "defs.h"
#include <concepts>

namespace laplace::math {
  template <typename type_>
  [[nodiscard]] constexpr auto get_zero() -> type_;

  template <typename type_>
  [[nodiscard]] constexpr auto get_unit() -> type_;

  template <typename type_>
  [[nodiscard]] constexpr auto get_size() -> size_t;

  template <typename type_>
  [[nodiscard]] constexpr auto get_row_count() -> size_t;

  template <typename type_>
  [[nodiscard]] constexpr auto get_column_count() -> size_t;

  template <typename type_>
  [[nodiscard]] constexpr auto get( //
      const type_ v,               //
      const size_t i) -> elem_type<type_>;

  template <typename type_>
  constexpr void set(           //
      type_ &                v, //
      const size_t           i, //
      const elem_type<type_> x);

  template <typename type_>
  concept vector_type = //
      requires {
    (void) get_size<type_>();
  }
  && //
      requires(type_ v, size_t i) {
    (void) get(v, i);
  }
  && //
      requires(type_ v, size_t i, scalar_type<type_> x) {
    set(v, i, x);
  };

  template <typename type_>
  concept matrix_type =     //
      vector_type<type_> && //
      requires {
    (void) get_row_count<type_>();
  }
  && //
      requires {
    (void) get_column_count<type_>();
  };

  template <typename type_>
  concept complex_type =    //
      vector_type<type_> && //
      get_size<type_>() == 2;

  template <typename type_>
  concept quaternion_type = //
      vector_type<type_> && //
      get_size<type_>() == 4;
}

#include "traits.impl.h"

#endif

/*  Copyright (c) 2021 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef laplace_math_matrix_h
#define laplace_math_matrix_h

#include "vector.h"

namespace laplace::math {
  template <sl::whole rows_, sl::whole cols_, typename type_>
  class matrix : public basic_vector<rows_ * cols_, type_> {
  public:
    static constexpr auto row_count    = rows_;
    static constexpr auto column_count = cols_;

    constexpr matrix(std::initializer_list<type_> values) noexcept;

    constexpr explicit matrix(type_ value) noexcept;

    using basic_vector<rows_ * cols_, type_>::basic_vector;
  };

  template <matrix_type type_>
  [[nodiscard]] constexpr auto get(type_ const &mat, sl::index row,
                                   sl::index col) noexcept
      -> elem_type<type_>;

  template <matrix_type type_>
  constexpr void set(type_ &mat, sl::index row, sl::index col,
                     elem_type<type_> val) noexcept;
}

#include "matrix.impl.h"

#endif

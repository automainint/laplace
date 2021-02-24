/*  laplace/math/matrix.h
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

#ifndef laplace_math_matrix_h
#define laplace_math_matrix_h

#include "vector.h"

namespace laplace::math {
  template <size_t rows_, size_t cols_, typename type_>
  class matrix : public basic_vector<rows_ * cols_, type_> {
  public:
    using basic_vector<rows_ * cols_, type_>::basic_vector;

    static constexpr auto row_count    = rows_;
    static constexpr auto column_count = cols_;
  };
}

#include "matrix.impl.h"

#endif

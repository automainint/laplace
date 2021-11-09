/*  Copyright (c) 2021 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef laplace_math_matrix_impl_h
#define laplace_math_matrix_impl_h

namespace laplace::math {
  template <sl::whole rows_, sl::whole cols_, typename type_>
  constexpr matrix<rows_, cols_, type_>::matrix(
      std::initializer_list<type_> values) noexcept {

    if (values.size() == 1) {
      if constexpr (rows_ == cols_) {
        for (sl::index i = 0; i < rows_; i++)
          this->v[i * cols_ + i] = *values.begin();

      } else {
        for (sl::index i = 0; i < this->size; i++)
          this->v[i] = *values.begin();
      }
    } else {
      for (sl::index i = 0; i < this->size && i < values.size(); i++)
        this->v[i] = values.begin()[i];
    }
  }

  template <sl::whole rows_, sl::whole cols_, typename type_>
  constexpr matrix<rows_, cols_, type_>::matrix(
      const type_ value) noexcept {

    for (sl::index i = 0; i < rows_; i++)
      this->v[i * cols_ + i] = value;
  }

  template <matrix_type type_>
  constexpr auto get(type_ const &mat, sl::index row,
                     sl::index col) noexcept -> elem_type<type_> {
    return get(mat, row * get_column_count<type_>() + col);
  }

  template <matrix_type type_>
  constexpr void set(type_ &mat, sl::index row, sl::index col,
                     elem_type<type_> val) noexcept {
    set(mat, row * get_column_count<type_>() + col, val);
  }
}

#endif

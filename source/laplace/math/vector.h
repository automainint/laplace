/*  Copyright (c) 2021 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef laplace_math_vector_h
#define laplace_math_vector_h

#include "basic.h"
#include <functional>
#include <initializer_list>

namespace laplace::math {
  template <sl::whole size_, typename type_>
  class basic_vector {
  public:
    using type = type_;

    static constexpr sl::whole size = size_;

    type v[size] = {};

    constexpr basic_vector() noexcept = default;
    constexpr basic_vector(std::initializer_list<type> v_) noexcept;

    constexpr explicit basic_vector(type x) noexcept;
  };

  template <sl::whole size_, typename type_>
  class vector : public basic_vector<size_, type_> {
  public:
    using basic_vector<size_, type_>::basic_vector;

    [[nodiscard]] constexpr auto operator[](sl::whole index) noexcept
        -> type_ &;
    [[nodiscard]] constexpr auto operator[](
        sl::whole index) const noexcept -> type_;
  };

  template <typename type_>
  class vector<2, type_> : public basic_vector<2, type_> {
  public:
    enum _indices : sl::whole { n_x = 0, n_y };

    using basic_vector<2, type_>::basic_vector;

    [[nodiscard]] constexpr auto x() noexcept -> type_ &;
    [[nodiscard]] constexpr auto y() noexcept -> type_ &;

    [[nodiscard]] constexpr auto x() const noexcept -> type_;
    [[nodiscard]] constexpr auto y() const noexcept -> type_;

    [[nodiscard]] constexpr auto operator[](sl::whole index) noexcept
        -> type_ &;

    [[nodiscard]] constexpr auto operator[](
        sl::whole index) const noexcept -> type_;
  };

  template <typename type_>
  class vector<3, type_> : public basic_vector<3, type_> {
  public:
    enum _indices : sl::whole { n_x = 0, n_y, n_z };

    using basic_vector<3, type_>::basic_vector;

    [[nodiscard]] constexpr auto x() noexcept -> type_ &;
    [[nodiscard]] constexpr auto y() noexcept -> type_ &;
    [[nodiscard]] constexpr auto z() noexcept -> type_ &;

    [[nodiscard]] constexpr auto x() const noexcept -> type_;
    [[nodiscard]] constexpr auto y() const noexcept -> type_;
    [[nodiscard]] constexpr auto z() const noexcept -> type_;

    [[nodiscard]] constexpr auto operator[](sl::whole index) noexcept
        -> type_ &;

    [[nodiscard]] constexpr auto operator[](
        sl::whole index) const noexcept -> type_;
  };

  template <typename type_>
  class vector<4, type_> : public basic_vector<4, type_> {
  public:
    enum _indices : sl::whole { n_x = 0, n_y, n_z, n_w };

    using basic_vector<4, type_>::basic_vector;

    [[nodiscard]] constexpr auto x() noexcept -> type_ &;
    [[nodiscard]] constexpr auto y() noexcept -> type_ &;
    [[nodiscard]] constexpr auto z() noexcept -> type_ &;
    [[nodiscard]] constexpr auto w() noexcept -> type_ &;

    [[nodiscard]] constexpr auto x() const noexcept -> type_;
    [[nodiscard]] constexpr auto y() const noexcept -> type_;
    [[nodiscard]] constexpr auto z() const noexcept -> type_;
    [[nodiscard]] constexpr auto w() const noexcept -> type_;

    [[nodiscard]] constexpr auto operator[](sl::whole index) noexcept
        -> type_ &;

    [[nodiscard]] constexpr auto operator[](
        sl::whole index) const noexcept -> type_;
  };

  template <sl::whole size_, typename type_>
  [[nodiscard]] constexpr auto operator==(
      vector<size_, type_> a, vector<size_, type_> b) noexcept
      -> bool;

  template <sl::whole size_, typename type_>
  [[nodiscard]] constexpr auto operator!=(
      vector<size_, type_> a, vector<size_, type_> b) noexcept
      -> bool;

  template <sl::whole size_, typename type_>
  [[nodiscard]] constexpr auto operator+(
      vector<size_, type_> a) noexcept -> vector<size_, type_>;

  template <sl::whole size_, typename type_>
  [[nodiscard]] constexpr auto operator-(
      vector<size_, type_> a) noexcept -> vector<size_, type_>;

  template <sl::whole size_, typename type_>
  [[nodiscard]] constexpr auto operator+(
      vector<size_, type_> a, vector<size_, type_> b) noexcept
      -> vector<size_, type_>;

  template <sl::whole size_, typename type_>
  [[nodiscard]] constexpr auto operator-(
      vector<size_, type_> a, vector<size_, type_> b) noexcept
      -> vector<size_, type_>;

  template <sl::whole size_, typename type_>
  [[nodiscard]] constexpr auto operator*(vector<size_, type_> a,
                                         type_ b) noexcept
      -> vector<size_, type_>;

  template <sl::whole size_, typename type_>
  [[nodiscard]] constexpr auto operator/(vector<size_, type_> a,
                                         type_ b) noexcept
      -> vector<size_, type_>;

  template <sl::whole size_, typename type_>
  [[nodiscard]] constexpr auto operator*(
      type_ a, vector<size_, type_> b) noexcept
      -> vector<size_, type_>;

  template <sl::whole size_, typename type_>
  [[nodiscard]] constexpr auto operator/(
      type_ a, vector<size_, type_> b) noexcept
      -> vector<size_, type_>;

  template <sl::whole size_, typename type_>
  constexpr auto operator+=(vector<size_, type_> &a,
                            vector<size_, type_>  b) noexcept
      -> vector<size_, type_> &;

  template <sl::whole size_, typename type_>
  constexpr auto operator-=(vector<size_, type_> &a,
                            vector<size_, type_>  b) noexcept
      -> vector<size_, type_> &;

  template <sl::whole size_, typename type_>
  constexpr auto operator*=(vector<size_, type_> &a, type_ b) noexcept
      -> vector<size_, type_> &;

  template <sl::whole size_, typename type_>
  constexpr auto operator/=(vector<size_, type_> &a, type_ b) noexcept
      -> vector<size_, type_> &;

  template <vector_type type_>
  [[nodiscard]] constexpr auto add(type_ a, type_ b) noexcept
      -> type_;

  template <vector_type type_>
  [[nodiscard]] constexpr auto sub(type_ a, type_ b) noexcept
      -> type_;

  template <vector_type type_>
  [[nodiscard]] constexpr auto mul(type_            a,
                                   elem_type<type_> b) noexcept
      -> type_;

  template <vector_type type_>
  [[nodiscard]] constexpr auto div(type_            a,
                                   elem_type<type_> b) noexcept
      -> type_;

  template <vector_type type_>
  [[nodiscard]] constexpr auto mul(elem_type<type_> a,
                                   type_ b) noexcept -> type_;

  template <vector_type type_>
  [[nodiscard]] constexpr auto div(elem_type<type_> a,
                                   type_ b) noexcept -> type_;

  template <vector_type type_>
  [[nodiscard]] constexpr auto mul_by_elem(type_ a, type_ b) noexcept
      -> type_;

  template <vector_type type_>
  [[nodiscard]] constexpr auto div_by_elem(type_ a, type_ b) noexcept
      -> type_;

  template <vector_type type_>
  [[nodiscard]] constexpr auto dot(type_ a, type_ b) noexcept
      -> elem_type<type_>;

  template <vector_type type_>
  [[nodiscard]] constexpr auto cross(type_ a, type_ b) noexcept
      -> type_;

  template <vector_type type_>
  [[nodiscard]] constexpr auto square_length(type_ a) noexcept
      -> elem_type<type_>;

  template <vector_type type_, typename op_>
  [[nodiscard]] constexpr auto op(type_ a, type_ b, op_ fn) noexcept
      -> type_;

  template <vector_type type_, typename op_>
  [[nodiscard]] constexpr auto op(type_            a,
                                  elem_type<type_> b,
                                  op_ fn) noexcept -> type_;

  template <vector_type type_, typename op_>
  [[nodiscard]] constexpr auto op(elem_type<type_> a,
                                  type_            b,
                                  op_ fn) noexcept -> type_;

  template <vector_type type_, typename add_, typename mul_>
  [[nodiscard]] constexpr auto dot(type_ a,
                                   type_ b,
                                   add_  fn_add,
                                   mul_  fn_mul) noexcept
      -> elem_type<type_>;

  template <vector_type type_, typename sub_, typename mul_>
  [[nodiscard]] constexpr auto cross(type_ a,
                                     type_ b,
                                     sub_  fn_sub,
                                     mul_  fn_mul) noexcept -> type_;

  template <vector_type type_, typename add_, typename mul_>
  [[nodiscard]] constexpr auto square_length(type_ a,
                                             add_  fn_add,
                                             mul_  fn_mul) noexcept
      -> elem_type<type_>;
}

#include "vector.impl.h"

#endif

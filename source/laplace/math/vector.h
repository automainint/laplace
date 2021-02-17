/*  laplace/math/vector.h
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

#ifndef __laplace__math_vector__
#define __laplace__math_vector__

#include "basic.h"
#include <functional>
#include <initializer_list>

namespace laplace::math {
  template <size_t size_, typename type_>
  class basic_vector {
  public:
    typedef type_ type;

    static constexpr size_t size = size_;

    type v[size] = {};

    constexpr basic_vector() noexcept;
    constexpr basic_vector(std::initializer_list<type> v_) noexcept;

    constexpr explicit basic_vector(const type x) noexcept;
  };

  template <size_t size_, typename type_>
  class vector : public basic_vector<size_, type_> {
  public:
    using basic_vector<size_, type_>::basic_vector;

    [[nodiscard]] constexpr auto operator[](size_t index) noexcept
        -> type_ &;
    [[nodiscard]] constexpr auto operator[](size_t index) const noexcept
        -> type_;
  };

  template <typename type_>
  class vector<2, type_> : public basic_vector<2, type_> {
  public:
    enum _indices : size_t { n_x = 0, n_y };

    using basic_vector<2, type_>::basic_vector;

    [[nodiscard]] constexpr auto x() noexcept -> type_ &;
    [[nodiscard]] constexpr auto y() noexcept -> type_ &;

    [[nodiscard]] constexpr auto x() const noexcept -> type_;
    [[nodiscard]] constexpr auto y() const noexcept -> type_;

    [[nodiscard]] constexpr auto operator[](size_t index) noexcept
        -> type_ &;

    [[nodiscard]] constexpr auto operator[](size_t index) const noexcept
        -> type_;
  };

  template <typename type_>
  class vector<3, type_> : public basic_vector<3, type_> {
  public:
    enum _indices : size_t { n_x = 0, n_y, n_z };

    using basic_vector<3, type_>::basic_vector;

    [[nodiscard]] constexpr auto x() noexcept -> type_ &;
    [[nodiscard]] constexpr auto y() noexcept -> type_ &;
    [[nodiscard]] constexpr auto z() noexcept -> type_ &;

    [[nodiscard]] constexpr auto x() const noexcept -> type_;
    [[nodiscard]] constexpr auto y() const noexcept -> type_;
    [[nodiscard]] constexpr auto z() const noexcept -> type_;

    [[nodiscard]] constexpr auto operator[](size_t index) noexcept
        -> type_ &;

    [[nodiscard]] constexpr auto operator[](size_t index) const noexcept
        -> type_;
  };

  template <typename type_>
  class vector<4, type_> : public basic_vector<4, type_> {
  public:
    enum _indices : size_t { n_x = 0, n_y, n_z, n_w };

    using basic_vector<4, type_>::basic_vector;

    [[nodiscard]] constexpr auto x() noexcept -> type_ &;
    [[nodiscard]] constexpr auto y() noexcept -> type_ &;
    [[nodiscard]] constexpr auto z() noexcept -> type_ &;
    [[nodiscard]] constexpr auto w() noexcept -> type_ &;

    [[nodiscard]] constexpr auto x() const noexcept -> type_;
    [[nodiscard]] constexpr auto y() const noexcept -> type_;
    [[nodiscard]] constexpr auto z() const noexcept -> type_;
    [[nodiscard]] constexpr auto w() const noexcept -> type_;

    [[nodiscard]] constexpr auto operator[](size_t index) noexcept
        -> type_ &;

    [[nodiscard]] constexpr auto operator[](size_t index) const noexcept
        -> type_;
  };

  template <size_t size_, typename type_>
  [[nodiscard]] constexpr auto operator==( //
      const vector<size_, type_> a,        //
      const vector<size_, type_> b         //
      ) noexcept -> bool;

  template <size_t size_, typename type_>
  [[nodiscard]] constexpr auto operator!=( //
      const vector<size_, type_> a,        //
      const vector<size_, type_> b         //
      ) noexcept -> bool;

  template <size_t size_, typename type_>
  [[nodiscard]] constexpr auto operator+( //
      const vector<size_, type_> a        //
      ) noexcept -> vector<size_, type_>;

  template <size_t size_, typename type_>
  [[nodiscard]] constexpr auto operator-( //
      const vector<size_, type_> a        //
      ) noexcept -> vector<size_, type_>;

  template <size_t size_, typename type_>
  [[nodiscard]] constexpr auto operator+( //
      const vector<size_, type_> a,       //
      const vector<size_, type_> b        //
      ) noexcept -> vector<size_, type_>;

  template <size_t size_, typename type_>
  [[nodiscard]] constexpr auto operator-( //
      const vector<size_, type_> a,       //
      const vector<size_, type_> b        //
      ) noexcept -> vector<size_, type_>;

  template <size_t size_, typename type_>
  [[nodiscard]] constexpr auto operator*( //
      const vector<size_, type_> a,       //
      const type_                b        //
      ) noexcept -> vector<size_, type_>;

  template <size_t size_, typename type_>
  [[nodiscard]] constexpr auto operator/( //
      const vector<size_, type_> a,       //
      const type_                b        //
      ) noexcept -> vector<size_, type_>;

  template <size_t size_, typename type_>
  [[nodiscard]] constexpr auto operator*( //
      const type_                a,       //
      const vector<size_, type_> b        //
      ) noexcept -> vector<size_, type_>;

  template <size_t size_, typename type_>
  [[nodiscard]] constexpr auto operator/( //
      const type_                a,       //
      const vector<size_, type_> b        //
      ) noexcept -> vector<size_, type_>;

  template <size_t size_, typename type_>
  constexpr auto operator+=(        //
      vector<size_, type_> &     a, //
      const vector<size_, type_> b  //
      ) noexcept -> vector<size_, type_> &;

  template <size_t size_, typename type_>
  constexpr auto operator-=(        //
      vector<size_, type_> &     a, //
      const vector<size_, type_> b  //
      ) noexcept -> vector<size_, type_> &;

  template <size_t size_, typename type_>
  constexpr auto operator*=(   //
      vector<size_, type_> &a, //
      const type_           b  //
      ) noexcept -> vector<size_, type_> &;

  template <size_t size_, typename type_>
  constexpr auto operator/=(   //
      vector<size_, type_> &a, //
      const type_           b  //
      ) noexcept -> vector<size_, type_> &;

  template <vector_type type_>
  [[nodiscard]] constexpr auto add( //
      const type_ a,                //
      const type_ b                 //
      ) noexcept -> type_;

  template <vector_type type_>
  [[nodiscard]] constexpr auto sub( //
      const type_ a,                //
      const type_ b                 //
      ) noexcept -> type_;

  template <vector_type type_>
  [[nodiscard]] constexpr auto mul( //
      const type_            a,     //
      const elem_type<type_> b      //
      ) noexcept -> type_;

  template <vector_type type_>
  [[nodiscard]] constexpr auto div( //
      const type_            a,     //
      const elem_type<type_> b      //
      ) noexcept -> type_;

  template <vector_type type_>
  [[nodiscard]] constexpr auto mul( //
      const elem_type<type_> a,     //
      const type_            b      //
      ) noexcept -> type_;

  template <vector_type type_>
  [[nodiscard]] constexpr auto div( //
      const elem_type<type_> a,     //
      const type_            b      //
      ) noexcept -> type_;

  template <vector_type type_>
  [[nodiscard]] constexpr auto mul_by_elem( //
      const type_ a,                        //
      const type_ b                         //
      ) noexcept -> type_;

  template <vector_type type_>
  [[nodiscard]] constexpr auto div_by_elem( //
      const type_ a,                        //
      const type_ b                         //
      ) noexcept -> type_;

  template <vector_type type_>
  [[nodiscard]] constexpr auto dot( //
      const type_ a,                //
      const type_ b                 //
      ) noexcept -> elem_type<type_>;

  template <vector_type type_>
  [[nodiscard]] constexpr auto cross( //
      const type_ a,                  //
      const type_ b                   //
      ) noexcept -> type_;

  template <vector_type type_>
  [[nodiscard]] constexpr auto square_length( //
      const type_ a                           //
      ) noexcept -> elem_type<type_>;
}

#include "vector.impl.h"

#endif

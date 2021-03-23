/*  laplace/math/vector.impl.h
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

#ifndef laplace_math_vector_impl_h
#define laplace_math_vector_impl_h

namespace laplace::math {
  template <size_t size_, typename type_>
  constexpr basic_vector<size_, type_>::basic_vector() noexcept {
  }

  template <size_t size_, typename type_>
  constexpr basic_vector<size_, type_>::basic_vector(
      std::initializer_list<type> v_) noexcept {

    if (v_.size() == 1) {
      for (size_t i = 0; i < this->size; i++) {
        this->v[i] = *v_.begin();
      }
    } else {
      for (size_t i = 0; i < v_.size() && i < this->size; i++) {
        this->v[i] = v_.begin()[i];
      }
    }
  }

  template <size_t size_, typename type_>
  constexpr basic_vector<size_, type_>::basic_vector( //
      const type x                                    //
      ) noexcept {

    for (size_t i = 0; i < this->size; i++) { this->v[i] = x; }
  }

  template <size_t size_, typename type_>
  constexpr auto vector<size_, type_>::operator[](size_t index) noexcept
      -> type_ & {

    if (index >= size_) {
      return *static_cast<type_ *>(nullptr);
    }

    return this->v[index];
  }

  template <size_t size_, typename type_>
  constexpr auto vector<size_, type_>::operator[](
      size_t index) const noexcept -> type_ {

    return get(*this, index);
  }

  template <typename type_>
  constexpr auto vector<2, type_>::x() noexcept -> type_ & {
    return this->v[this->n_x];
  }

  template <typename type_>
  constexpr auto vector<2, type_>::y() noexcept -> type_ & {
    return this->v[this->n_y];
  }

  template <typename type_>
  constexpr auto vector<2, type_>::x() const noexcept -> type_ {
    return this->v[this->n_x];
  }

  template <typename type_>
  constexpr auto vector<2, type_>::y() const noexcept -> type_ {
    return this->v[this->n_y];
  }

  template <typename type_>
  constexpr auto vector<2, type_>::operator[](size_t index) noexcept
      -> type_ & {

    if (index >= 2) {
      return *static_cast<type_ *>(nullptr);
    }

    return this->v[index];
  }

  template <typename type_>
  constexpr auto vector<2, type_>::operator[](size_t index) const noexcept
      -> type_ {

    return get(*this, index);
  }

  template <typename type_>
  constexpr auto vector<3, type_>::x() noexcept -> type_ & {
    return this->v[this->n_x];
  }

  template <typename type_>
  constexpr auto vector<3, type_>::y() noexcept -> type_ & {
    return this->v[this->n_y];
  }

  template <typename type_>
  constexpr auto vector<3, type_>::z() noexcept -> type_ & {
    return this->v[this->n_z];
  }

  template <typename type_>
  constexpr auto vector<3, type_>::x() const noexcept -> type_ {
    return this->v[this->n_x];
  }

  template <typename type_>
  constexpr auto vector<3, type_>::y() const noexcept -> type_ {
    return this->v[this->n_y];
  }

  template <typename type_>
  constexpr auto vector<3, type_>::z() const noexcept -> type_ {
    return this->v[this->n_z];
  }

  template <typename type_>
  constexpr auto vector<3, type_>::operator[](size_t index) noexcept
      -> type_ & {

    if (index >= 3) {
      return *static_cast<type_ *>(nullptr);
    }

    return this->v[index];
  }

  template <typename type_>
  constexpr auto vector<3, type_>::operator[](size_t index) const noexcept
      -> type_ {

    return get(*this, index);
  }

  template <typename type_>
  constexpr auto vector<4, type_>::x() noexcept -> type_ & {
    return this->v[this->n_x];
  }

  template <typename type_>
  constexpr auto vector<4, type_>::y() noexcept -> type_ & {
    return this->v[this->n_y];
  }

  template <typename type_>
  constexpr auto vector<4, type_>::z() noexcept -> type_ & {
    return this->v[this->n_z];
  }

  template <typename type_>
  constexpr auto vector<4, type_>::w() noexcept -> type_ & {
    return this->v[this->n_w];
  }

  template <typename type_>
  constexpr auto vector<4, type_>::x() const noexcept -> type_ {
    return this->v[this->n_x];
  }

  template <typename type_>
  constexpr auto vector<4, type_>::y() const noexcept -> type_ {
    return this->v[this->n_y];
  }

  template <typename type_>
  constexpr auto vector<4, type_>::z() const noexcept -> type_ {
    return this->v[this->n_z];
  }

  template <typename type_>
  constexpr auto vector<4, type_>::w() const noexcept -> type_ {
    return this->v[this->n_w];
  }

  template <typename type_>
  constexpr auto vector<4, type_>::operator[](size_t index) noexcept
      -> type_ & {

    if (index >= 4) {
      return *static_cast<type_ *>(nullptr);
    }

    return this->v[index];
  }

  template <typename type_>
  constexpr auto vector<4, type_>::operator[](size_t index) const noexcept
      -> type_ {

    return get(*this, index);
  }

  template <size_t size_, typename type_>
  constexpr auto operator==(        //
      const vector<size_, type_> a, //
      const vector<size_, type_> b  //
      ) noexcept -> bool {

    return equals(a, b);
  }

  template <size_t size_, typename type_>
  constexpr auto operator!=(        //
      const vector<size_, type_> a, //
      const vector<size_, type_> b  //
      ) noexcept -> bool {

    return !equals(a, b);
  }

  template <size_t size_, typename type_>
  constexpr auto operator+(        //
      const vector<size_, type_> a //
      ) noexcept -> vector<size_, type_> {

    return a;
  }

  template <size_t size_, typename type_>
  constexpr auto operator-(        //
      const vector<size_, type_> a //
      ) noexcept -> vector<size_, type_> {

    vector<size_, type_> x;
    for (size_t i = 0; i < size_; i++) { x.v[i] = -a.v[i]; }
    return x;
  }

  template <size_t size_, typename type_>
  constexpr auto operator+(         //
      const vector<size_, type_> a, //
      const vector<size_, type_> b  //
      ) noexcept -> vector<size_, type_> {

    return add(a, b);
  }

  template <size_t size_, typename type_>
  constexpr auto operator-(         //
      const vector<size_, type_> a, //
      const vector<size_, type_> b  //
      ) noexcept -> vector<size_, type_> {

    return sub(a, b);
  }

  template <size_t size_, typename type_>
  constexpr auto operator*(         //
      const vector<size_, type_> a, //
      const type_                b  //
      ) noexcept -> vector<size_, type_> {

    return mul(a, b);
  }

  template <size_t size_, typename type_>
  constexpr auto operator/(         //
      const vector<size_, type_> a, //
      const type_                b  //
      ) noexcept -> vector<size_, type_> {

    return div(a, b);
  }

  template <size_t size_, typename type_>
  constexpr auto operator*(         //
      const type_                a, //
      const vector<size_, type_> b  //
      ) noexcept -> vector<size_, type_> {

    return mul(a, b);
  }

  template <size_t size_, typename type_>
  constexpr auto operator/(         //
      const type_                a, //
      const vector<size_, type_> b  //
      ) noexcept -> vector<size_, type_> {

    return div(a, b);
  }

  template <size_t size_, typename type_>
  constexpr auto operator+=(        //
      vector<size_, type_> &     a, //
      const vector<size_, type_> b  //
      ) noexcept -> vector<size_, type_> & {

    return a = add(a, b);
  }

  template <size_t size_, typename type_>
  constexpr auto operator-=(        //
      vector<size_, type_> &     a, //
      const vector<size_, type_> b  //
      ) noexcept -> vector<size_, type_> & {

    return a = sub(a, b);
  }

  template <size_t size_, typename type_>
  constexpr auto operator*=(   //
      vector<size_, type_> &a, //
      const type_           b  //
      ) noexcept -> vector<size_, type_> & {

    return a = mul(a, b);
  }

  template <size_t size_, typename type_>
  constexpr auto operator/=(   //
      vector<size_, type_> &a, //
      const type_           b  //
      ) noexcept -> vector<size_, type_> & {

    return a = div(a, b);
  }

  template <vector_type type_>
  constexpr auto add( //
      const type_ a,  //
      const type_ b   //
      ) noexcept -> type_ {

    auto v = type_ {};

    for (size_t i = 0; i < get_size<type_>(); i++) {
      set(v, i, get(a, i) + get(b, i));
    }

    return v;
  }

  template <vector_type type_>
  constexpr auto sub( //
      const type_ a,  //
      const type_ b   //
      ) noexcept -> type_ {

    auto v = type_ {};

    for (size_t i = 0; i < get_size<type_>(); i++) {
      set(v, i, get(a, i) - get(b, i));
    }

    return v;
  }

  template <vector_type type_>
  constexpr auto mul(           //
      const type_            a, //
      const elem_type<type_> b  //
      ) noexcept -> type_ {

    auto v = type_ {};

    for (size_t i = 0; i < get_size<type_>(); i++) {
      set(v, i, get(a, i) * b);
    }

    return v;
  }

  template <vector_type type_>
  constexpr auto div(           //
      const type_            a, //
      const elem_type<type_> b  //
      ) noexcept -> type_ {

    auto v = type_ {};

    for (size_t i = 0; i < get_size<type_>(); i++) {
      set(v, i, get(a, i) / b);
    }

    return v;
  }

  template <vector_type type_>
  constexpr auto mul(           //
      const elem_type<type_> a, //
      const type_            b  //
      ) noexcept -> type_ {

    auto v = type_ {};

    for (size_t i = 0; i < get_size<type_>(); i++) {
      set(v, i, a * get(b, i));
    }

    return v;
  }

  template <vector_type type_>
  constexpr auto div(           //
      const elem_type<type_> a, //
      const type_            b  //
      ) noexcept -> type_ {

    auto v = type_ {};

    for (size_t i = 0; i < get_size<type_>(); i++) {
      set(v, i, a / get(b, i));
    }

    return v;
  }

  template <vector_type type_>
  constexpr auto mul_by_elem( //
      const type_ a,          //
      const type_ b           //
      ) noexcept -> type_ {

    auto v = type_ {};

    for (size_t i = 0; i < get_size<type_>(); i++) {
      set(v, i, get(a, i) * get(b, i));
    }

    return v;
  }

  template <vector_type type_>
  constexpr auto div_by_elem( //
      const type_ a,          //
      const type_ b           //
      ) noexcept -> type_ {

    auto v = type_ {};

    for (size_t i = 0; i < get_size<type_>(); i++) {
      set(v, i, get(a, i) / get(b, i));
    }

    return v;
  }

  template <vector_type type_>
  constexpr auto dot( //
      const type_ a,  //
      const type_ b   //
      ) noexcept -> elem_type<type_> {

    auto x = elem_type<type_> {};

    for (size_t i = 0; i < get_size<type_>(); i++) {
      x += get(a, i) * get(b, i);
    }

    return x;
  }

  template <vector_type type_>
  constexpr auto cross( //
      const type_ a,    //
      const type_ b     //
      ) noexcept -> type_ {

    static_assert(get_size<type_>() == 3);

    auto c = type_ {};

    set(c, 0, get(a, 1) * get(b, 2) - get(a, 2) * get(b, 1));
    set(c, 1, get(a, 2) * get(b, 0) - get(a, 0) * get(b, 2));
    set(c, 2, get(a, 0) * get(b, 1) - get(a, 1) * get(b, 0));

    return c;
  }

  template <vector_type type_>
  constexpr auto square_length( //
      const type_ a             //
      ) noexcept -> elem_type<type_> {

    return dot(a, a);
  }
}

#endif

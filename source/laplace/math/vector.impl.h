/*  Copyright (c) 2021 Mitya Selivanov
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
  template <sl::whole size_, typename type_>
  constexpr basic_vector<size_, type_>::basic_vector(
      std::initializer_list<type> v_) noexcept {

    if (v_.size() == 1) {
      for (sl::whole i = 0; i < this->size; i++) {
        this->v[i] = *v_.begin();
      }
    } else {
      for (sl::whole i = 0; i < v_.size() && i < this->size; i++) {
        this->v[i] = v_.begin()[i];
      }
    }
  }

  template <sl::whole size_, typename type_>
  constexpr basic_vector<size_, type_>::basic_vector(
      const type x) noexcept {

    for (sl::whole i = 0; i < this->size; i++) { this->v[i] = x; }
  }

  template <sl::whole size_, typename type_>
  constexpr auto vector<size_, type_>::operator[](
      sl::whole index) noexcept -> type_ & {

    if (index >= size_) {
      return *static_cast<type_ *>(nullptr);
    }

    return this->v[index];
  }

  template <sl::whole size_, typename type_>
  constexpr auto vector<size_, type_>::operator[](
      sl::whole index) const noexcept -> type_ {

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
  constexpr auto vector<2, type_>::operator[](
      sl::whole index) noexcept -> type_ & {

    if (index >= 2) {
      return *static_cast<type_ *>(nullptr);
    }

    return this->v[index];
  }

  template <typename type_>
  constexpr auto vector<2, type_>::operator[](
      sl::whole index) const noexcept -> type_ {

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
  constexpr auto vector<3, type_>::operator[](
      sl::whole index) noexcept -> type_ & {

    if (index >= 3) {
      return *static_cast<type_ *>(nullptr);
    }

    return this->v[index];
  }

  template <typename type_>
  constexpr auto vector<3, type_>::operator[](
      sl::whole index) const noexcept -> type_ {

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
  constexpr auto vector<4, type_>::operator[](
      sl::whole index) noexcept -> type_ & {

    if (index >= 4) {
      return *static_cast<type_ *>(nullptr);
    }

    return this->v[index];
  }

  template <typename type_>
  constexpr auto vector<4, type_>::operator[](
      sl::whole index) const noexcept -> type_ {

    return get(*this, index);
  }

  template <sl::whole size_, typename type_>
  constexpr auto operator==(const vector<size_, type_> a,
                            const vector<size_, type_> b) noexcept
      -> bool {

    return equals(a, b);
  }

  template <sl::whole size_, typename type_>
  constexpr auto operator!=(const vector<size_, type_> a,
                            const vector<size_, type_> b) noexcept
      -> bool {

    return !equals(a, b);
  }

  template <sl::whole size_, typename type_>
  constexpr auto operator+(const vector<size_, type_> a) noexcept
      -> vector<size_, type_> {

    return a;
  }

  template <sl::whole size_, typename type_>
  constexpr auto operator-(const vector<size_, type_> a) noexcept
      -> vector<size_, type_> {

    vector<size_, type_> x;
    for (sl::whole i = 0; i < size_; i++) { x.v[i] = -a.v[i]; }
    return x;
  }

  template <sl::whole size_, typename type_>
  constexpr auto operator+(const vector<size_, type_> a,
                           const vector<size_, type_> b) noexcept
      -> vector<size_, type_> {

    return add(a, b);
  }

  template <sl::whole size_, typename type_>
  constexpr auto operator-(const vector<size_, type_> a,
                           const vector<size_, type_> b) noexcept
      -> vector<size_, type_> {

    return sub(a, b);
  }

  template <sl::whole size_, typename type_>
  constexpr auto operator*(const vector<size_, type_> a,
                           const type_                b) noexcept
      -> vector<size_, type_> {

    return mul(a, b);
  }

  template <sl::whole size_, typename type_>
  constexpr auto operator/(const vector<size_, type_> a,
                           const type_                b) noexcept
      -> vector<size_, type_> {

    return div(a, b);
  }

  template <sl::whole size_, typename type_>
  constexpr auto operator*(const type_                a,
                           const vector<size_, type_> b) noexcept
      -> vector<size_, type_> {

    return mul(a, b);
  }

  template <sl::whole size_, typename type_>
  constexpr auto operator/(const type_                a,
                           const vector<size_, type_> b) noexcept
      -> vector<size_, type_> {

    return div(a, b);
  }

  template <sl::whole size_, typename type_>
  constexpr auto operator+=(vector<size_, type_>      &a,
                            const vector<size_, type_> b) noexcept
      -> vector<size_, type_> & {

    return a = add(a, b);
  }

  template <sl::whole size_, typename type_>
  constexpr auto operator-=(vector<size_, type_>      &a,
                            const vector<size_, type_> b) noexcept
      -> vector<size_, type_> & {

    return a = sub(a, b);
  }

  template <sl::whole size_, typename type_>
  constexpr auto operator*=(vector<size_, type_> &a,
                            const type_           b) noexcept
      -> vector<size_, type_> & {

    return a = mul(a, b);
  }

  template <sl::whole size_, typename type_>
  constexpr auto operator/=(vector<size_, type_> &a,
                            const type_           b) noexcept
      -> vector<size_, type_> & {

    return a = div(a, b);
  }

  template <vector_type type_>
  constexpr auto add(type_ const a, type_ const b) noexcept -> type_ {

    auto v = type_ {};

    for (sl::whole i = 0; i < get_size<type_>(); i++)
      set(v, i, get(a, i) + get(b, i));

    return v;
  }

  template <vector_type type_>
  constexpr auto sub(type_ const a, type_ const b) noexcept -> type_ {

    auto v = type_ {};

    for (sl::whole i = 0; i < get_size<type_>(); i++)
      set(v, i, get(a, i) - get(b, i));

    return v;
  }

  template <vector_type type_>
  constexpr auto mul(type_ const a, elem_type<type_> const b) noexcept
      -> type_ {

    auto v = type_ {};

    for (sl::whole i = 0; i < get_size<type_>(); i++)
      set(v, i, get(a, i) * b);

    return v;
  }

  template <vector_type type_>
  constexpr auto div(type_ const a, elem_type<type_> const b) noexcept
      -> type_ {

    auto v = type_ {};

    for (sl::whole i = 0; i < get_size<type_>(); i++)
      set(v, i, get(a, i) / b);

    return v;
  }

  template <vector_type type_>
  constexpr auto mul(elem_type<type_> const a, type_ const b) noexcept
      -> type_ {

    auto v = type_ {};

    for (sl::whole i = 0; i < get_size<type_>(); i++)
      set(v, i, a * get(b, i));

    return v;
  }

  template <vector_type type_>
  constexpr auto div(elem_type<type_> const a, type_ const b) noexcept
      -> type_ {

    auto v = type_ {};

    for (sl::whole i = 0; i < get_size<type_>(); i++)
      set(v, i, a / get(b, i));

    return v;
  }

  template <vector_type type_>
  constexpr auto mul_by_elem(type_ const a, type_ const b) noexcept
      -> type_ {

    auto v = type_ {};

    for (sl::whole i = 0; i < get_size<type_>(); i++)
      set(v, i, get(a, i) * get(b, i));

    return v;
  }

  template <vector_type type_>
  constexpr auto div_by_elem(type_ const a, type_ const b) noexcept
      -> type_ {

    auto v = type_ {};

    for (sl::whole i = 0; i < get_size<type_>(); i++)
      set(v, i, get(a, i) / get(b, i));

    return v;
  }

  template <vector_type type_>
  constexpr auto dot(type_ const a, type_ const b) noexcept
      -> elem_type<type_> {

    auto x = elem_type<type_> {};

    for (sl::whole i = 0; i < get_size<type_>(); i++)
      x += get(a, i) * get(b, i);

    return x;
  }

  template <vector_type type_>
  constexpr auto cross(type_ const a, type_ const b) noexcept
      -> type_ {

    static_assert(get_size<type_>() == 3);

    auto c = type_ {};

    set(c, 0, get(a, 1) * get(b, 2) - get(a, 2) * get(b, 1));
    set(c, 1, get(a, 2) * get(b, 0) - get(a, 0) * get(b, 2));
    set(c, 2, get(a, 0) * get(b, 1) - get(a, 1) * get(b, 0));

    return c;
  }

  template <vector_type type_>
  constexpr auto square_length(type_ const a) noexcept
      -> elem_type<type_> {

    return dot(a, a);
  }

  template <vector_type type_, typename op_>
  constexpr auto op(type_ const a, type_ const b, op_ fn) noexcept
      -> type_ {

    auto v = type_ {};

    for (sl::whole i = 0; i < get_size<type_>(); i++)
      set(v, i, fn(get(a, i), get(b, i)));

    return v;
  }

  template <vector_type type_, typename op_>
  constexpr auto op(type_ const a, elem_type<type_> const b,
                    op_ fn) noexcept -> type_ {

    auto v = type_ {};

    for (sl::whole i = 0; i < get_size<type_>(); i++)
      set(v, i, fn(get(a, i), b));

    return v;
  }

  template <vector_type type_, typename op_>
  constexpr auto op(elem_type<type_> const a, type_ const b,
                    op_ fn) noexcept -> type_ {

    auto v = type_ {};

    for (sl::whole i = 0; i < get_size<type_>(); i++)
      set(v, i, fn(a, get(b, i)));

    return v;
  }

  template <vector_type type_, typename add_, typename mul_>
  constexpr auto dot(type_ a, type_ b, add_ _add, mul_ _mul) noexcept
      -> elem_type<type_> {

    auto x = elem_type<type_> {};

    for (sl::whole i = 0; i < get_size<type_>(); i++)
      x = _add(x, _mul(get(a, i), get(b, i)));

    return x;
  }

  template <vector_type type_, typename sub_, typename mul_>
  constexpr auto cross(type_ a, type_ b, sub_ _sub,
                       mul_ _mul) noexcept -> type_ {

    static_assert(get_size<type_>() == 3);

    auto c = type_ {};

    set(c, 0,
        _sub(_mul(get(a, 1), get(b, 2)), _mul(get(a, 2), get(b, 1))));

    set(c, 1,
        _sub(_mul(get(a, 2), get(b, 0)), _mul(get(a, 0), get(b, 2))));

    set(c, 2,
        _sub(_mul(get(a, 0), get(b, 1)), _mul(get(a, 1), get(b, 0))));

    return c;
  }

  template <vector_type type_, typename add_, typename mul_>
  constexpr auto square_length(type_ const a, add_ _add,
                               mul_ _mul) noexcept
      -> elem_type<type_> {
    return dot(a, a, _add, _mul);
  }
}

#endif

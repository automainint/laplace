/*  laplace/math/traits.impl.h
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

#ifndef laplace_math_traits_impl_h
#define laplace_math_traits_impl_h

namespace laplace::math {
  template <typename type_>
  struct _helper {
    static constexpr auto size = type_::size;
    static constexpr auto zero = type_(_helper<elem_type<type_>>::zero);
    static constexpr auto unit = type_(_helper<elem_type<type_>>::unit);
  };

  template <typename type_>
  requires is_scalar<type_> struct _helper<type_> {
    static constexpr sl::whole size = 1;
    static constexpr auto      zero = type_(0);
    static constexpr auto      unit = type_(1);
  };

  template <typename type_>
  struct _get_helper {
    [[nodiscard]] static constexpr auto get(const type_     v,
                                            const sl::index i)
        -> elem_type<type_> {

      if (i < get_size<type_>()) {
        return v.v[i];
      }

      return {};
    }
  };

  template <typename type_>
  requires is_scalar<type_> struct _get_helper<type_> {

    [[nodiscard]] static constexpr auto get(const type_     v,
                                            const sl::index i)
        -> type_ {

      if (i == 0) {
        return v;
      }

      return {};
    }
  };

  template <typename type_>
  struct _set_helper {
    static constexpr void set(type_ &v, const sl::index i,
                              const elem_type<type_> x) {

      if (i < get_size<type_>()) {
        v.v[i] = x;
      }
    }
  };

  template <typename type_>
  requires is_scalar<type_> struct _set_helper<type_> {

    static constexpr void set(type_ &v, const sl::index i,
                              const type_ x) {

      if (i == 0) {
        v = x;
      }
    }
  };

  template <typename type_>
  constexpr auto get_zero() -> type_ {
    return _helper<type_>::zero;
  }

  template <typename type_>
  constexpr auto get_unit() -> type_ {
    return _helper<type_>::unit;
  }

  template <typename type_>
  constexpr auto get_size() -> sl::whole {
    return _helper<type_>::size;
  }

  template <typename type_>
  constexpr auto get_row_count() -> sl::whole {
    return type_::row_count;
  }

  template <typename type_>
  constexpr auto get_column_count() -> sl::whole {
    return type_::column_count;
  }

  template <typename type_>
  constexpr auto get(const type_ v, const sl::index i)
      -> elem_type<type_> {

    return _get_helper<type_>::get(v, i);
  }

  template <typename type_>
  constexpr void set(type_ &v, const sl::index i,
                     const elem_type<type_> x) {

    _set_helper<type_>::set(v, i, x);
  }
}

#endif

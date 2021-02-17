/*  laplace/math/basic.impl.h
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

#ifndef __laplace__math_basic_impl__
#define __laplace__math_basic_impl__

namespace laplace::math {
  template <typename type_>
  struct _equals_helper {

    using elem_helper = _equals_helper<elem_type<type_>>;

    static constexpr auto equals( //
        const type_ a,            //
        const type_ b             //
        ) noexcept -> bool {

      for (size_t i = 0; i < get_size<type_>(); i++) {
        const auto x0 = get(a, i);
        const auto x1 = get(b, i);

        if (!elem_helper::equals(x0, x1))
          return false;
      }

      return true;
    }

    static constexpr auto equals(        //
        const type_              a,      //
        const type_              b,      //
        const scalar_type<type_> epsilon //
        ) noexcept -> bool {

      for (size_t i = 0; i < get_size<type_>(); i++) {
        const auto x0 = get(a, i);
        const auto x1 = get(b, i);

        if (!elem_helper::equals(x0, x1, epsilon))
          return false;
      }

      return true;
    }
  };

  template <typename type_>
  requires is_scalar<type_> //
      struct _equals_helper<type_> {

    static constexpr auto equals( //
        const type_ a,            //
        const type_ b             //
        ) noexcept -> bool {

      return a == b;
    }

    static constexpr auto equals( //
        const type_ a,            //
        const type_ b,            //
        const type_ epsilon       //
        ) noexcept -> bool {

      return std::abs(a - b) <= epsilon;
    }
  };

  template <typename type_>
  constexpr auto equals( //
      const type_ a,     //
      const type_ b      //
      ) noexcept -> bool {

    return _equals_helper<type_>::equals(a, b);
  }

  template <typename type_>
  constexpr auto equals(               //
      const type_              a,      //
      const type_              b,      //
      const scalar_type<type_> epsilon //
      ) noexcept -> bool {

    return _equals_helper<type_>::equals(a, b, epsilon);
  }

  template <typename type_, typename arg_>
  constexpr auto round(const arg_ t) noexcept -> type_ {
    return static_cast<type_>(t);
  }

  template <typename type_, typename arg_>
  constexpr auto round(const arg_ t) noexcept -> type_
      requires std::is_integral_v<type_> {
    return static_cast<type_>(std::round(t));
  }

  template <typename type_, typename sqrt_>
  struct _length_helper {

    static inline auto length(const type_ v, sqrt_ sqrt) {
      return round<scalar_type<type_>>(sqrt(square_length(v)));
    }
  };

  template <typename type_, typename sqrt_>
  requires is_scalar<type_> //
      struct _length_helper<type_, sqrt_> {

    static inline auto length(const type_ v, sqrt_ sqrt) {
      return v;
    }
  };

  template <typename type_, typename sqrt_>
  inline auto length(  //
      const type_ v,   //
      sqrt_       sqrt //
      ) -> scalar_type<type_> {

    return _length_helper<type_, sqrt_>::length(v, sqrt);
  }

  template <typename type_>
  inline auto length( //
      const type_ v   //
      ) -> scalar_type<type_> {

    return length(v, [](const auto x) {
      return std::sqrtl(x);
    });
  }

  template <typename type_>
  struct _lerp_helper {

    static inline auto lerp( //
        const type_     a,   //
        const type_     b,   //
        const realmax_t t) {

      auto x = type_ {};

      for (size_t i = 0; i < get_size<type_>(); i++) {

        set(x, i,
            _lerp_helper<scalar_type<type_>>::lerp( //
                get(a, i),                          //
                get(b, i),                          //
                t)                                  //
        );
      }

      return x;
    }
  };

  template <typename type_>
  requires is_scalar<type_> //
      struct _lerp_helper<type_> {

    static inline auto lerp( //
        const type_     a,   //
        const type_     b,   //
        const realmax_t t) {

      return round<type_>(std::lerp( //
          static_cast<realmax_t>(a), //
          static_cast<realmax_t>(b), //
          t                          //
          ));
    }
  };

  template <typename type_>
  constexpr auto lerp(       //
      const type_     begin, //
      const type_     end,   //
      const realmax_t t      //
      ) noexcept -> type_ {

    return _lerp_helper<type_>::lerp(begin, end, t);
  }
}

#endif

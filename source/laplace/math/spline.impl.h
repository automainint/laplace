/*  laplace/math/spline.impl.h
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

#ifndef __laplace__math_spline_impl__
#define __laplace__math_spline_impl__

namespace laplace::math {
  template <typename time_, typename type_>
  inline void spline<time_, type_>::set_linear(type_ begin,
                                               type_ end) {
    this->m_piece = linear_piece { begin, end };
  }

  template <typename time_, typename type_>
  inline void spline<time_, type_>::set_bezier(
      std::array<type_, 4> points) {
    this->m_piece = bezier_piece { points };
  }

  template <typename time_, typename type_>
  inline void spline<time_, type_>::set_hermite(
      type_ begin, type_ end, std::array<type_, 2> tangents) {
    this->m_piece = hermite_piece { begin, end, tangents };
  }

  template <typename time_, typename type_>
  inline auto spline<time_, type_>::solve(time_ t) const
      -> type_ {
    return this->is_linear()
               ? linear(std::get<0>(this->m_piece).begin,
                        std::get<0>(this->m_piece).end, t)
           :

           this->is_bezier()
               ? bezier(std::get<1>(this->m_piece).points, t)
           :

           this->is_hermite()
               ? hermite(std::get<2>(this->m_piece).begin,
                         std::get<2>(this->m_piece).end,
                         std::get<2>(this->m_piece).tangents, t)
               :

               type_(0);
  }

  template <typename time_, typename type_>
  inline auto spline<time_, type_>::get_begin() -> type_ {
    return this->is_linear() ? std::get<0>(this->m_piece).begin :

           this->is_bezier()
               ? std::get<1>(this->m_piece).points[0]
           :

           this->is_hermite() ? std::get<2>(this->m_piece).begin
                              :

                              type_(0);
  }

  template <typename time_, typename type_>
  inline auto spline<time_, type_>::get_end() -> type_ {
    return this->is_linear() ? std::get<0>(this->m_piece).end :

           this->is_bezier()
               ? std::get<1>(this->m_piece).points[3]
           :

           this->is_hermite() ? std::get<2>(this->m_piece).end
                              :

                              type_(0);
  }

  template <typename time_, typename type_>
  inline auto spline<time_, type_>::operator()(time_ t) const
      -> type_ {
    return this->solve(t);
  }

  template <typename time_, typename type_>
  inline auto spline<time_, type_>::is_linear() const -> bool {
    return this->m_piece.index() == 0;
  }

  template <typename time_, typename type_>
  inline auto spline<time_, type_>::is_bezier() const -> bool {
    return this->m_piece.index() == 1;
  }

  template <typename time_, typename type_>
  inline auto spline<time_, type_>::is_hermite() const -> bool {
    return this->m_piece.index() == 2;
  }

  template <typename time_, typename type_>
  inline auto spline<time_, type_>::linear(type_ begin,
                                           type_ end, time_ t)
      -> type_ {
    return lerp(begin, end, t);
  }

  template <typename time_, typename type_>
  inline auto spline<time_, type_>::bezier(
      std::array<type_, 4> points, time_ t) -> type_ {
    auto t2 = t * t;
    auto t3 = t2 * t;

    auto to  = 1. - t;
    auto to2 = to * to;
    auto to3 = to2 * to;

    return type_(to3) * points[0] + type_(to2 * t) * points[1] +
           type_(to * t2) * points[2] + type_(t3) * points[3];
  }

  template <typename time_, typename type_>
  inline auto spline<time_, type_>::hermite(
      type_ begin, type_ end, std::array<type_, 2> tangents,
      time_ t) -> type_ {
    auto t2 = t * t;
    auto t3 = t2 * t;

    auto a = (time_(2) * t3 - time_(3) * t2 + time_(1));
    auto b = (t3 - time_(2) * t2 + t);
    auto c = (time_(-2) * t3 + time_(3) * t2);
    auto d = (t3 - t2);

    return type_(a) * begin + type_(b) * tangents[0] +
           type_(c * end) + type_(d) * tangents[1];
  }

  template <typename time_, typename type_>
  inline auto spline<time_, type_>::flat()
      -> const spline<time_, type_> & {
    static spline<time_, type_> s;
    return s;
  }
}

#endif

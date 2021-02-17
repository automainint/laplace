/*  laplace/math/spline.h
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

#ifndef __laplace__math_spline__
#define __laplace__math_spline__

#include "quaternion.h"
#include <array>
#include <variant>

namespace laplace::math {
  /*  Spline.
   *
   *  Linear, Cubic Bezier or Cubic Hermite interpolation.
   */
  template <typename time_, typename type_>
  class spline {
  public:
    struct linear_piece {
      type_ begin = get_zero<type_>();
      type_ end   = get_unit<type_>();
    };

    struct bezier_piece {
      std::array<type_, 4> points;
    };

    struct hermite_piece {
      type_                begin;
      type_                end;
      std::array<type_, 2> tangents;
    };

    using piece =
        std::variant<linear_piece, bezier_piece, hermite_piece>;

    void set_linear(type_ begin, type_ end);
    void set_bezier(std::array<type_, 4> points);
    void set_hermite(type_ begin, type_ end,
                     std::array<type_, 2> tangents);

    auto solve(time_ t) const -> type_;

    auto get_begin() -> type_;
    auto get_end() -> type_;

    auto operator()(time_ t) const -> type_;

    auto is_linear() const -> bool;
    auto is_bezier() const -> bool;
    auto is_hermite() const -> bool;

    static auto linear(type_ begin, type_ end, time_ t) -> type_;
    static auto bezier(std::array<type_, 4> points, time_ t)
        -> type_;
    static auto hermite(type_ begin, type_ end,
                        std::array<type_, 2> tangents, time_ t)
        -> type_;

    /*  f(x) = x
     */
    static auto flat() -> const spline<time_, type_> &;

  private:
    piece m_piece;
  };
}

#include "spline.impl.h"

#endif

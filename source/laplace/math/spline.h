#pragma once

#include "quaternion.h"
#include <array>
#include <variant>

namespace laplace::math
{
    /*  Spline.
     *
     *  Linear, Cubic Bezier or Cubic Hermite interpolation.
     */
    template <typename timeval, typename vecval>
    class spline
    {
    public:
        struct linear_piece
        {
            vecval begin = vecval(traits<vecval>::zero);
            vecval end = vecval(traits<vecval>::unit);
        };

        struct bezier_piece
        {
            std::array<vecval, 4> points;
        };

        struct hermite_piece
        {
            vecval begin;
            vecval end;
            std::array<vecval, 2> tangents;
        };

        using piece = std::variant<
            linear_piece,
            bezier_piece,
            hermite_piece
        >;

        void set_linear(vecval begin, vecval end);
        void set_bezier(std::array<vecval, 4> points);
        void set_hermite(vecval begin, vecval end, std::array<vecval, 2> tangents);

        auto solve(timeval t) const -> vecval;

        auto get_begin() -> vecval;
        auto get_end() -> vecval;

        auto operator()(timeval t) const -> vecval;

        auto is_linear() const -> bool;
        auto is_bezier() const -> bool;
        auto is_hermite() const -> bool;

        static auto linear(vecval begin, vecval end, timeval t) -> vecval;
        static auto bezier(std::array<vecval, 4> points, timeval t) -> vecval;
        static auto hermite(vecval begin, vecval end, std::array<vecval, 2> tangents, timeval t) -> vecval;

        /*  f(x) = x
         */
        static auto flat() -> const spline<timeval, vecval> &;

    private:
        piece m_piece;
    };
}

#include "spline.impl.h"

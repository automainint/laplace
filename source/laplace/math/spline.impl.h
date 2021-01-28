#pragma once

namespace laplace::math
{
    template <typename timeval, typename vecval>
    inline void spline<timeval, vecval>::set_linear(vecval begin, vecval end)
    {
        this->m_piece = linear_piece { begin, end };
    }

    template <typename timeval, typename vecval>
    inline void spline<timeval, vecval>::set_bezier(std::array<vecval, 4> points)
    {
        this->m_piece = bezier_piece { points };
    }

    template <typename timeval, typename vecval>
    inline void spline<timeval, vecval>::set_hermite(vecval begin, vecval end, std::array<vecval, 2> tangents)
    {
        this->m_piece = hermite_piece { begin, end, tangents };
    }

    template <typename timeval, typename vecval>
    inline auto spline<timeval, vecval>::solve(timeval t) const -> vecval
    {
        return
            this->is_linear() ?
            linear(
                std::get<0>(this->m_piece).begin,
                std::get<0>(this->m_piece).end,
                t
            ) :

            this->is_bezier() ?
            bezier(
                std::get<1>(this->m_piece).points,
                t
            ) :

            this->is_hermite() ?
            hermite(
                std::get<2>(this->m_piece).begin,
                std::get<2>(this->m_piece).end,
                std::get<2>(this->m_piece).tangents,
                t
            ) :

            vecval(0);
    }

    template <typename timeval, typename vecval>
    inline auto spline<timeval, vecval>::get_begin() -> vecval
    {
        return
            this->is_linear() ?
            std::get<0>(this->m_piece).begin :

            this->is_bezier() ?
            std::get<1>(this->m_piece).points[0] :

            this->is_hermite() ?
            std::get<2>(this->m_piece).begin :

            vecval(0);
    }

    template <typename timeval, typename vecval>
    inline auto spline<timeval, vecval>::get_end() -> vecval
    {
        return
            this->is_linear() ?
            std::get<0>(this->m_piece).end :

            this->is_bezier() ?
            std::get<1>(this->m_piece).points[3] :

            this->is_hermite() ?
            std::get<2>(this->m_piece).end :

            vecval(0);
    }

    template <typename timeval, typename vecval>
    inline auto spline<timeval, vecval>::operator()(timeval t) const -> vecval
    {
        return this->solve(t);
    }

    template <typename timeval, typename vecval>
    inline auto spline<timeval, vecval>::is_linear() const -> bool
    {
        return this->m_piece.index() == 0;
    }

    template <typename timeval, typename vecval>
    inline auto spline<timeval, vecval>::is_bezier() const -> bool
    {
        return this->m_piece.index() == 1;
    }

    template <typename timeval, typename vecval>
    inline auto spline<timeval, vecval>::is_hermite() const -> bool
    {
        return this->m_piece.index() == 2;
    }

    template <typename timeval, typename vecval>
    inline auto spline<timeval, vecval>::linear(vecval begin, vecval end, timeval t) -> vecval
    {
        return lerp(begin, end, t);
    }

    template <typename timeval, typename vecval>
    inline auto spline<timeval, vecval>::bezier(std::array<vecval, 4> points, timeval t) -> vecval
    {
        auto t2 = t * t;
        auto t3 = t2 * t;

        auto to = 1. - t;
        auto to2 = to * to;
        auto to3 = to2 * to;

        return vecval(to3) * points[0] + vecval(to2 * t) * points[1] + vecval(to * t2) * points[2] + vecval(t3) * points[3];
    }

    template <typename timeval, typename vecval>
    inline auto spline<timeval, vecval>::hermite(vecval begin, vecval end, std::array<vecval, 2> tangents, timeval t) -> vecval
    {
        auto t2 = t * t;
        auto t3 = t2 * t;

        auto a = (timeval(2) * t3 - timeval(3) * t2 + timeval(1));
        auto b = (t3 - timeval(2) * t2 + t);
        auto c = (timeval(-2) * t3 + timeval(3) * t2);
        auto d = (t3 - t2);

        return vecval(a) * begin + vecval(b) * tangents[0] + vecval(c * end) + vecval(d) * tangents[1];
    }

    template <typename timeval, typename vecval>
    inline auto spline<timeval, vecval>::flat() -> const spline<timeval, vecval> &
    {
        static spline<timeval, vecval> s;
        return s;
    }
}

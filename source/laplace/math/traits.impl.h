#pragma once

namespace laplace::math
{
    template <typename vecval>
    inline void traits<vecval>::set(vecval &v, size_t i, vecval value)
    {
        if (i == 0)
        {
            v = value;
        }
    }

    template <typename vecval>
    inline auto traits<vecval>::get(const vecval &v, size_t i) -> vecval
    {
        return i == 0 ? v : vecval();
    }

    template <size_t valcount, typename vecval>
    inline void traits<vector<valcount, vecval>>::set(vector<valcount, vecval> &v, size_t i, vecval value)
    {
        v[i] = value;
    }

    template <size_t valcount, typename vecval>
    inline auto traits<vector<valcount, vecval>>::get(const vector<valcount, vecval> &v, size_t i) -> vecval
    {
        return v[i];
    }

    template <typename vecval>
    inline void traits<complex<vecval>>::set(complex<vecval> &v, size_t i, vecval value)
    {
        v[i] = value;
    }

    template <typename vecval>
    inline auto traits<complex<vecval>>::get(const complex<vecval> &v, size_t i) -> vecval
    {
        return v[i];
    }

    template <typename vecval>
    inline void traits<quaternion<vecval>>::set(quaternion<vecval> &v, size_t i, vecval value)
    {
        v[i] = value;
    }

    template <typename vecval>
    inline auto traits<quaternion<vecval>>::get(const quaternion<vecval> &v, size_t i) -> vecval
    {
        return v[i];
    }

    template <size_t rowcount, size_t colcount, typename vecval>
    inline void traits<matrix<rowcount, colcount, vecval>>::set(matrix<rowcount, colcount, vecval> &v, size_t i, vecval value)
    {
        v.v[i] = value;
    }

    template <size_t rowcount, size_t colcount, typename vecval>
    inline auto traits<matrix<rowcount, colcount, vecval>>::get(const matrix<rowcount, colcount, vecval> &v, size_t i) -> vecval
    {
        return v.v[i];
    }

    template <typename vecval>
    inline auto linear_equals(const vecval &a, const vecval &b) -> bool
    {
        return a == b;
    }

    template <>
    inline auto linear_equals(const float &a, const float &b) -> bool
    {
        return
            a - b <= std::numeric_limits<float>::epsilon() &&
            b - a <= std::numeric_limits<float>::epsilon();
    }

    template <>
    inline auto linear_equals(const double &a, const double &b) -> bool
    {
        return
            a - b <= std::numeric_limits<double>::epsilon() &&
            b - a <= std::numeric_limits<double>::epsilon();
    }

    template <>
    inline auto linear_equals(const long double &a, const long double &b) -> bool
    {
        return
            a - b <= std::numeric_limits<long double>::epsilon() &&
            b - a <= std::numeric_limits<long double>::epsilon();
    }

    template <typename vecval>
    inline auto linear_equals(const vecval &a, const vecval &b, const vecval &epsilon) -> bool
    {
        return a - b <= epsilon && b - a <= epsilon;
    }

    template <typename vecval>
    inline auto equals(const vecval &a, const vecval &b) -> bool
    {
        for (size_t i = 0; i < traits<vecval>::count; i++)
        {
            if (!linear_equals(traits<vecval>::get(a, i), traits<vecval>::get(b, i)))
            {
                return false;
            }
        }

        return true;
    }

    template <typename vecval>
    inline auto equals(const vecval &a, const vecval &b, const typename traits<vecval>::type &epsilon) -> bool
    {
        for (size_t i = 0; i < traits<vecval>::count; i++)
        {
            if (!linear_equals(traits<vecval>::get(a, i), traits<vecval>::get(b, i), epsilon))
            {
                return false;
            }
        }

        return true;
    }

    template <typename vecval>
    inline auto lerp(const vecval &begin, const vecval &end, realmax_t t) -> vecval
    {
        return vecval(begin + (end - begin) * vecval(t));
    }

    template <typename vecval>
    inline auto lerp(const quaternion<vecval> &begin, const quaternion<vecval> &end, realmax_t t) -> vecval
    {
        return slerp(begin, end, t);
    }
}

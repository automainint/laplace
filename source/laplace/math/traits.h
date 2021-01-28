/*  Utilities that allows to manipulate simple types,
 *  vectors, complex numbers, quaternions and matrices
 *  with same functions (operators).
 */

#pragma once

#include <cstdint>
#include <limits>

namespace laplace::math
{
    using realmax_t = long double;

    template <size_t valcount, typename vecval>
    class vector;

    template <size_t rowcount, size_t colcount, typename vecval>
    class matrix;

    template <typename vecval>
    class complex;

    template <typename vecval>
    class quaternion;

    template <typename vecval>
    class traits
    {
    public:
        using type = vecval;

        static constexpr size_t count = 1;
        static constexpr vecval zero = type(0);
        static constexpr vecval unit = type(1);

        static void set(vecval &v, size_t i, vecval value);
        static auto get(const vecval &v, size_t i) -> vecval;
    };

    template <size_t valcount, typename vecval>
    class traits<vector<valcount, vecval>>
    {
    public:
        using type = vecval;

        static constexpr size_t count = valcount;
        static constexpr vector<valcount, vecval> zero = vector<valcount, vecval>(type(0));
        static constexpr vector<valcount, vecval> unit = vector<valcount, vecval>(type(1));

        static void set(vector<valcount, vecval> &v, size_t i, vecval value);
        static auto get(const vector<valcount, vecval> &v, size_t i) -> vecval;
    };

    template <typename vecval>
    class traits<complex<vecval>>
    {
    public:
        using type = vecval;

        static constexpr size_t count = 2;
        static constexpr complex<vecval> zero = complex<vecval>(type(0));
        static constexpr complex<vecval> unit = complex<vecval>(type(1), type(0));

        static void set(complex<vecval> &v, size_t i, vecval value);
        static auto get(const complex<vecval> &v, size_t i) -> vecval;
    };

    template <typename vecval>
    class traits<quaternion<vecval>>
    {
    public:
        using type = vecval;

        static constexpr size_t count = 4;
        static constexpr quaternion<vecval> zero = quaternion<vecval>(type(0));
        static constexpr quaternion<vecval> unit = quaternion<vecval>(type(1));

        static void set(quaternion<vecval> &v, size_t i, vecval value);
        static auto get(const quaternion<vecval> &v, size_t i) -> vecval;
    };

    template <size_t rowcount, size_t colcount, typename vecval>
    class traits<matrix<rowcount, colcount, vecval>>
    {
    public:
        using type = vecval;

        static constexpr size_t count = rowcount * colcount;
        static constexpr matrix<rowcount, colcount, vecval> zero = matrix<rowcount, colcount, vecval>(type(0));
        static constexpr matrix<rowcount, colcount, vecval> unit = matrix<rowcount, colcount, vecval>(type(1));

        static void set(matrix<rowcount, colcount, vecval> &v, size_t i, vecval value);
        static auto get(const matrix<rowcount, colcount, vecval> &v, size_t i) -> vecval;
    };

    template <typename vecval>
    auto linear_equals(const vecval &a, const vecval &b) -> bool;

    template <typename vecval>
    auto linear_equals(const vecval &a, const vecval &b, const vecval &epsilon) -> bool;

    template <typename vecval>
    auto equals(const vecval &a, const vecval &b) -> bool;

    template <typename vecval>
    auto equals(const vecval &a, const vecval &b, const typename traits<vecval>::type &epsilon) -> bool;

    /*  Linear interpolation.
     */
    template <typename vecval>
    auto lerp(const vecval &begin, const vecval &end, realmax_t t) -> vecval;
}

#include "traits.impl.h"

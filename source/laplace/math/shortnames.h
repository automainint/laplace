#pragma once

#include <cstdint>

namespace laplace::math
{
    template <size_t valcount, typename vecval>
    class vector;

    template <size_t rowcount, size_t colcount, typename vecval>
    class matrix;

    template <typename vecval>
    class complex;

    template <typename vecval>
    class quaternion;
    
    namespace shortnames
    {
        using real = double;

        using vec2 = vector<2, real>;
        using vec3 = vector<3, real>;
        using vec4 = vector<4, real>;

        using mat2 = matrix<2, 2, real>;
        using mat3 = matrix<3, 3, real>;
        using mat4 = matrix<4, 4, real>;

        using comp = complex<real>;
        using quat = quaternion<real>;
    }
}

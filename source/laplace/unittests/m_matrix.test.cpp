#include "../math/matrix.h"
#include "../math/traits.h"
#include <gtest/gtest.h>
#include <random>

using namespace laplace;
using namespace math;
using namespace std;

TEST(laplace_math, matrix_add)
{
    matrix<4, 4, float> a = {
        1, 2, 3, 4,
        5, 6, 7, 8,
        -1, -2, -3, -4,
        -5, -6, -7, -8
    };

    matrix<4, 4, float> b = {
        -1, -2, -3, -4,
        -5, -6, -7, -8,
        1, 2, 3, 4,
        5, 6, 7, 8
    };

    matrix<4, 4, float> c = {
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0
    };

    auto d = add(a, b);

    EXPECT_TRUE(equals(c, d));
}

TEST(laplace_math, matrix_subtract)
{
    matrix<4, 4, float> a = {
        1, 2, 3, 4,
        5, 6, 7, 8,
        -1, -2, -3, -4,
        -5, -6, -7, -8
    };

    matrix<4, 4, float> b = {
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0
    };

    auto c = subtract(a, a);

    EXPECT_TRUE(equals(b, c));
}

TEST(laplace_math, matrix_add_sub)
{
    default_random_engine rnd;
    uniform_real_distribution<float> dst(-100, 100);

    for (size_t i = 0; i < 10; i++)
    {
        matrix<4, 4, float> a, b;

        for (size_t i = 0; i < 16; i++)
        {
            a.v[i] = dst(rnd);
            b.v[i] = dst(rnd);
        }

        auto c = subtract(add(a, b), b);

        EXPECT_TRUE(equals(a, c, 1e-5f));
    }
}

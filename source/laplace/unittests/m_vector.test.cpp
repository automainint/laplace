#include "../math/vector.h"
#include <gtest/gtest.h>

using namespace laplace;
using namespace math;

TEST(laplace_math, vector_comparison)
{
    auto a = vector<3, double>(1.0, 2.0, 3.0);
    auto b = vector<3, double>(1.0, 2.0, 3.0);
    auto c = vector<3, double>(2.0, 3.0, 1.0);

    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a == c);
    EXPECT_FALSE(a != b);
    EXPECT_TRUE(a != c);
}

TEST(laplace_math, vector_dot)
{
    auto a = vector<3, double>(2.0, 2.0, 2.0);
    auto b = vector<3, double>(-2.0, 2.0, -1.0);

    auto d0 = dot(a, b);
    constexpr auto d1 = 2.0 * (-2) + 2 * 2 + (2 * -1);

    EXPECT_NEAR(d0, d1, 1e-10);
}

TEST(laplace_math, vector_cross)
{
    auto a = vector<3, double>(2.0, 2.0, 2.0);
    auto b = vector<3, double>(-2.0, 2.0, -1.0);

    auto d0 = cross(a, b);

    auto d1 = vector<3, double>(
        2.0 * (-1) - 2 * 2,
        2.0 * (-2) - 2 * (-1),
        2.0 * 2 - 2 * (-2));

    EXPECT_TRUE(d0 == d1);
}

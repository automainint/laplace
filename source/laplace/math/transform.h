#pragma once

#include "vector.h"
#include "matrix.h"
#include "complex.h"
#include "quaternion.h"
#include <cmath>
#include <utility>

#undef near
#undef far

namespace laplace::math
{
    template <typename vecval>
    complex<vecval> rotation_complex(vecval angle);

    template <typename vecval>
    quaternion<vecval> rotation_quaternion(const vector<3, vecval> &axis, vecval angle);

    template <typename vecval>
    quaternion<vecval> rotation_quaternion(vecval yaw, vecval pitch, vecval roll);

    template <typename vecval>
    matrix<4, 4, vecval> translation_matrix(const vector<3, vecval> &v);

    template <typename vecval>
    matrix<4, 4, vecval> rotation_matrix(const vector<3, vecval> &axis, vecval angle);

    template <typename vecval>
    matrix<3, 3, vecval> rotation_matrix3x3(const vector<3, vecval> &axis, vecval angle);

    template <typename vecval>
    matrix<4, 4, vecval> scale_matrix(const vector<3, vecval> &v);

    template <typename vecval>
    matrix<4, 4, vecval> rotation_matrix(vecval yaw, vecval pitch, vecval roll);

    template <typename vecval>
    matrix<3, 3, vecval> rotation_matrix3x3(vecval yaw, vecval pitch, vecval roll);

    template <typename vecval>
    matrix<4, 4, vecval> mirror_x_matrix();

    template <typename vecval>
    matrix<4, 4, vecval> mirror_y_matrix();

    template <typename vecval>
    matrix<4, 4, vecval> mirror_z_matrix();

    template <typename vecval>
    matrix<4, 4, vecval> ortho_matrix(vecval left, vecval right, vecval bottom, vecval top, vecval near, vecval far);

    template <typename vecval>
    matrix<4, 4, vecval> frustum_matrix(vecval left, vecval right, vecval bottom, vecval top, vecval near, vecval far);

    template <typename vecval>
    matrix<4, 4, vecval> perspective_matrix(vecval fovy, vecval aspect, vecval near, vecval far);

    template <typename vecval>
    matrix<4, 4, vecval> look_at_matrix(const vector<3, vecval> &center, const vector<3, vecval> &up);

    template <typename vecval>
    matrix<3, 3, vecval> look_at_matrix3x3(const vector<3, vecval> &center, const vector<3, vecval> &up);

    template <typename vecval>
    matrix<4, 4, vecval> look_at_matrix(const vector<3, vecval> &eye, const vector<3, vecval> &center, const vector<3, vecval> &up);

    template <typename vecval>
    matrix<3, 3, vecval> look_at_matrix3x3(const vector<3, vecval> &eye, const vector<3, vecval> &center, const vector<3, vecval> &up);

    template <typename vecval>
    auto model_transform(
        const vector<3, vecval> &position,
        const matrix<3, 3, vecval> &rotation,
        const vector<3, vecval> &scale
    ) -> matrix<4, 4, vecval>;

    template <typename vecval>
    auto full_transform(
        const matrix<4, 4, vecval> &model_transform,
        const matrix<4, 4, vecval> &view_transform,
        const matrix<4, 4, vecval> &projection
    ) -> matrix<4, 4, vecval>;
}

#include "transform.impl.h"

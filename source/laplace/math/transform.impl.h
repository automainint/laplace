#pragma once

namespace laplace::math
{
    template <typename vecval>
    inline complex<vecval> rotation_complex(vecval angle)
    {
        return complex<vecval>(vecval(cos(angle)), vecval(sin(angle)));
    }

    template <typename vecval>
    inline quaternion<vecval> rotation_quaternion(const vector<3, vecval> &axis, vecval angle)
    {
        quaternion<vecval> result;

        vecval alpha = angle * vecval(0.5);
        vecval sine = vecval(sin(alpha));

        result.x = axis.x * sine;
        result.y = axis.y * sine;
        result.z = axis.z * sine;
        result.w = vecval(cos(alpha));

        return std::move(result);
    }

    template <typename vecval>
    inline quaternion<vecval> rotation_quaternion(vecval yaw, vecval pitch, vecval roll)
    {
        quaternion<vecval> result;

        vecval cx = vecval(cos(yaw * 0.5));
        vecval cy = vecval(cos(pitch * 0.5));
        vecval cz = vecval(cos(roll * 0.5));

        vecval sx = vecval(sin(yaw * 0.5));
        vecval sy = vecval(sin(pitch * 0.5));
        vecval sz = vecval(sin(roll * 0.5));

        result.x = sx * cy * cz - cx * sy * sz;
        result.y = cx * sy * cz + sx * cy * sz;
        result.z = cx * cy * sz - sx * sy * cz;
        result.w = cx * cy * cz + sx * sy * sz;

        return std::move(result);
    }

    template <typename vecval>
    inline matrix<4, 4, vecval> translation_matrix(const vector<3, vecval> &v)
    {
        return {
            traits<vecval>::unit, traits<vecval>::zero, traits<vecval>::zero, traits<vecval>::zero,
            traits<vecval>::zero, traits<vecval>::unit, traits<vecval>::zero, traits<vecval>::zero,
            traits<vecval>::zero, traits<vecval>::zero, traits<vecval>::unit, traits<vecval>::zero,
            v.x, v.y, v.z, traits<vecval>::unit
        };
    }

    template <typename vecval>
    inline matrix<4, 4, vecval> rotation_matrix(const vector<3, vecval> &axis, vecval angle)
    {
        vecval cosine = vecval(cos(angle));
        vecval sine = vecval(sin(angle));

        return {
            axis.x * axis.x + (traits<vecval>::unit - axis.x * axis.x) * cosine,
            axis.x * axis.y * (traits<vecval>::unit - cosine) + axis.z * sine,
            axis.x * axis.z * (traits<vecval>::unit - cosine) - axis.y * sine,
            traits<vecval>::zero,
            axis.x * axis.y * (traits<vecval>::unit - cosine) - axis.z * sine,
            axis.y * axis.y + (traits<vecval>::unit - axis.y * axis.y) * cosine,
            axis.y * axis.z * (traits<vecval>::unit - cosine) + axis.x * sine,
            traits<vecval>::zero,
            axis.x * axis.z * (traits<vecval>::unit - cosine) + axis.y * sine,
            axis.y * axis.z * (traits<vecval>::unit - cosine) - axis.x * sine,
            axis.z * axis.z + (traits<vecval>::unit - axis.z * axis.z) * cosine,
            traits<vecval>::zero,
            traits<vecval>::zero, traits<vecval>::zero, traits<vecval>::zero, traits<vecval>::unit
        };
    }

    template <typename vecval>
    inline matrix<3, 3, vecval> rotation_matrix3x3(const vector<3, vecval> &axis, vecval angle)
    {
        vecval cosine = vecval(cos(angle));
        vecval sine = vecval(sin(angle));

        return {
            axis.x * axis.x + (traits<vecval>::unit - axis.x * axis.x) * cosine,
            axis.x * axis.y * (traits<vecval>::unit - cosine) + axis.z * sine,
            axis.x * axis.z * (traits<vecval>::unit - cosine) - axis.y * sine,
            axis.x * axis.y * (traits<vecval>::unit - cosine) - axis.z * sine,
            axis.y * axis.y + (traits<vecval>::unit - axis.y * axis.y) * cosine,
            axis.y * axis.z * (traits<vecval>::unit - cosine) + axis.x * sine,
            axis.x * axis.z * (traits<vecval>::unit - cosine) + axis.y * sine,
            axis.y * axis.z * (traits<vecval>::unit - cosine) - axis.x * sine,
            axis.z * axis.z + (traits<vecval>::unit - axis.z * axis.z) * cosine
        };
    }

    template <typename vecval>
    inline matrix<4, 4, vecval> scale_matrix(const vector<3, vecval> &v)
    {
        return {
            v.x, vecval(), vecval(), vecval(),
            vecval(), v.y, vecval(), vecval(),
            vecval(), vecval(), v.z, vecval(),
            vecval(), vecval(), vecval(), traits<vecval>::unit
        };
    }

    template <typename vecval>
    inline matrix<4, 4, vecval> rotation_matrix(vecval yaw, vecval pitch, vecval roll)
    {
        vecval cx = vecval(cos(yaw));
        vecval cy = vecval(cos(pitch));
        vecval cz = vecval(cos(roll));

        vecval sx = vecval(sin(yaw));
        vecval sy = vecval(sin(pitch));
        vecval sz = vecval(sin(roll));

        return {
            cy * cz, cy * sz, -sy, traits<vecval>::zero,
            cz * sy * sx - sz * cx, sz * sy * sx + cz * cx, cy * sx, traits<vecval>::zero,
            cz * sy * cx + sz * sx, sz * sy * cx - cz * sx, cy * cx, traits<vecval>::zero,
            traits<vecval>::zero, traits<vecval>::zero, traits<vecval>::zero, traits<vecval>::unit
        };
    }

    template <typename vecval>
    inline matrix<3, 3, vecval> rotation_matrix3x3(vecval yaw, vecval pitch, vecval roll)
    {
        vecval cx = vecval(cos(yaw));
        vecval cy = vecval(cos(pitch));
        vecval cz = vecval(cos(roll));

        vecval sx = vecval(sin(yaw));
        vecval sy = vecval(sin(pitch));
        vecval sz = vecval(sin(roll));

        return {
            cy * cz, cy * sz, -sy,
            cz * sy * sx - sz * cx, sz * sy * sx + cz * cx, cy * sx,
            cz * sy * cx + sz * sx, sz * sy * cx - cz * sx, cy * cx
        };
    }

    template <typename vecval>
    inline matrix<4, 4, vecval> mirror_x_matrix()
    {
        return {
            vecval(-1), traits<vecval>::zero, traits<vecval>::zero, traits<vecval>::zero,
            traits<vecval>::zero, traits<vecval>::unit, traits<vecval>::zero, traits<vecval>::zero,
            traits<vecval>::zero, traits<vecval>::zero, traits<vecval>::unit, traits<vecval>::zero,
            traits<vecval>::zero, traits<vecval>::zero, traits<vecval>::zero, traits<vecval>::unit
        };
    }

    template <typename vecval>
    inline matrix<4, 4, vecval> mirror_y_matrix()
    {
        return {
            traits<vecval>::unit, traits<vecval>::zero, traits<vecval>::zero, traits<vecval>::zero,
            traits<vecval>::zero, vecval(-1), traits<vecval>::zero, traits<vecval>::zero,
            traits<vecval>::zero, traits<vecval>::zero, traits<vecval>::unit, traits<vecval>::zero,
            traits<vecval>::zero, traits<vecval>::zero, traits<vecval>::zero, traits<vecval>::unit
        };
    }

    template <typename vecval>
    inline matrix<4, 4, vecval> mirror_z_matrix()
    {
        return {
            traits<vecval>::unit, traits<vecval>::zero, traits<vecval>::zero, traits<vecval>::zero,
            traits<vecval>::zero, traits<vecval>::unit, traits<vecval>::zero, traits<vecval>::zero,
            traits<vecval>::zero, traits<vecval>::zero, vecval(-1), traits<vecval>::zero,
            traits<vecval>::zero, traits<vecval>::zero, traits<vecval>::zero, traits<vecval>::unit
        };
    }

    template <typename vecval>
    inline matrix<4, 4, vecval> ortho_matrix(vecval left, vecval right, vecval bottom, vecval top, vecval near, vecval far)
    {
        matrix<4, 4, vecval> result(traits<vecval>::unit);

        result.m[0][0] = vecval(2) / (right - left);
        result.m[1][1] = vecval(2) / (top - bottom);
        result.m[2][2] = -traits<vecval>::unit / (far - near);
        result.m[3][0] = -(right + left) / (right - left);
        result.m[3][1] = -(top + bottom) / (top - bottom);
        result.m[3][2] = -near / (far - near);

        return std::move(result);
    }

    template <typename vecval>
    inline matrix<4, 4, vecval> frustum_matrix(vecval left, vecval right, vecval bottom, vecval top, vecval near, vecval far)
    {
        matrix<4, 4, vecval> result(traits<vecval>::zero);

        result.m[0][0] = (vecval(2) * near) / (right - left);
        result.m[1][1] = (vecval(2) * near) / (top - bottom);
        result.m[2][0] = (right + left) / (right - left);
        result.m[2][1] = (top + bottom) / (top - bottom);
        result.m[2][2] = far / (near - far);
        result.m[2][3] = vecval(-1);
        result.m[3][2] = -(far * near) / (far - near);

        return std::move(result);
    }

    template <typename vecval>
    inline matrix<4, 4, vecval> perspective_matrix(vecval fovy, vecval aspect, vecval near, vecval far)
    {
        const vecval tan_half_fovy = tan(fovy / vecval(2));

        matrix<4, 4, vecval> result(traits<vecval>::zero);

        result.m[0][0] = traits<vecval>::unit / (aspect * tan_half_fovy);
        result.m[1][1] = traits<vecval>::unit / (tan_half_fovy);
        result.m[2][2] = far / (near - far);
        result.m[2][3] = -traits<vecval>::unit;
        result.m[3][2] = -(far * near) / (far - near);

        return std::move(result);
    }

    template <typename vecval>
    inline matrix<4, 4, vecval> look_at_matrix(const vector<3, vecval> &center, const vector<3, vecval> &up)
    {
        vector<3, vecval> f = normal(center);
        vector<3, vecval> s = normal(cross(f, up));
        vector<3, vecval> u = cross(s, f);

        return {
            s.x, s.y, s.z, traits<vecval>::zero,
            u.x, u.y, u.z, traits<vecval>::zero,
            -f.x, -f.y, -f.z, traits<vecval>::zero,
            traits<vecval>::zero, traits<vecval>::zero, traits<vecval>::zero, traits<vecval>::unit
        };
    }

    template <typename vecval>
    inline matrix<3, 3, vecval> look_at_matrix3x3(const vector<3, vecval> &center, const vector<3, vecval> &up)
    {
        vector<3, vecval> f = normal(center);
        vector<3, vecval> s = normal(cross(f, up));
        vector<3, vecval> u = cross(s, f);

        return {
            s.x, s.y, s.z,
            u.x, u.y, u.z,
            -f.x, -f.y, -f.z
        };
    }

    template <typename vecval>
    inline matrix<4, 4, vecval> look_at_matrix(const vector<3, vecval> &eye, const vector<3, vecval> &center, const vector<3, vecval> &up)
    {
        vector<3, vecval> f = normal(center - eye);
        vector<3, vecval> s = normal(cross(f, up));
        vector<3, vecval> u = cross(s, f);

        return {
            s.x, s.y, s.z, -dot(s, eye),
            u.x, u.y, u.z, -dot(u, eye),
            -f.x, -f.y, -f.z, dot(f, eye),
            traits<vecval>::zero, traits<vecval>::zero, traits<vecval>::zero, traits<vecval>::unit
        };
    }

    template <typename vecval>
    inline matrix<3, 3, vecval> look_at_matrix3x3(const vector<3, vecval> &eye, const vector<3, vecval> &center, const vector<3, vecval> &up)
    {
        vector<3, vecval> f = normal(center - eye);
        vector<3, vecval> s = normal(cross(f, up));
        vector<3, vecval> u = cross(s, f);

        return {
            s.x, s.y, s.z,
            u.x, u.y, u.z,
            -f.x, -f.y, -f.z
        };
    }

    template <typename vecval>
    inline auto model_transform(
        const vector<3, vecval> &position,
        const matrix<3, 3, vecval> &rotation,
        const vector<3, vecval> &scale
    ) -> matrix<4, 4, vecval>
    {
        /*  TODO
         */
        return { };
    }

    template <typename vecval>
    inline auto full_transform(
        const matrix<4, 4, vecval> &model_transform,
        const matrix<4, 4, vecval> &view_transform,
        const matrix<4, 4, vecval> &projection
    ) -> matrix<4, 4, vecval>
    {
        /*  TODO
         */
        return { };
    }
}

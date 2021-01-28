#pragma once

#include <algorithm>
#include <cassert>

namespace laplace::math
{
    template <size_t rowcount, size_t colcount, typename vecval>
    constexpr matrix<rowcount, colcount, vecval>::matrix(std::initializer_list<vecval> values) : v()
    {
        assert(values.size() == rowcount * colcount);

        std::move(values.begin(), values.end(), this->v);
    }

    template <size_t rowcount, size_t colcount, typename vecval>
    constexpr matrix<rowcount, colcount, vecval>::matrix(vecval d) : v()
    {
        for (size_t i = 0; i < colcount; i++)
        {
            this->m[i][i] = d;

            for (size_t j = i + 1; j < colcount; j++)
            {
                this->m[i][j] = this->m[j][i] = traits<vecval>::zero;
            }
        }
    }

    template <size_t rowcount, size_t colcount, typename vecval>
    constexpr matrix<rowcount, colcount, vecval>::matrix(const vecval *v) : v()
    {
        for (size_t i = 0; i < colcount * rowcount; i++)
        {
            this->v[i] = v[i];
        }
    }

    template <size_t rowcount, size_t colcount, typename vecval>
    inline matrix<rowcount, colcount, vecval> &matrix<rowcount, colcount, vecval>::operator +=(const matrix<rowcount, colcount, vecval> &mat)
    {
        for (size_t i = 0; i < colcount * rowcount; i++)
        {
            this->v[i] += mat.v[i];
        }

        return *this;
    }

    template <size_t rowcount, size_t colcount, typename vecval>
    inline matrix<rowcount, colcount, vecval> &matrix<rowcount, colcount, vecval>::operator -=(const matrix<rowcount, colcount, vecval> &mat)
    {
        for (size_t i = 0; i < colcount * rowcount; i++)
        {
            this->v[i] -= mat.v[i];
        }

        return *this;
    }

    template <size_t rowcount, size_t colcount, typename vecval>
    inline matrix<rowcount, colcount, vecval> &matrix<rowcount, colcount, vecval>::operator *=(vecval d)
    {
        for (size_t i = 0; i < colcount * rowcount; i++)
        {
            this->v[i] *= d;
        }
    }

    template <size_t rowcount, size_t colcount, typename vecval>
    inline matrix<rowcount, colcount, vecval> &matrix<rowcount, colcount, vecval>::operator /=(vecval d)
    {
        for (size_t i = 0; i < colcount * rowcount; i++)
        {
            this->v[i] /= d;
        }
    }

    template <size_t rowcount, size_t colcount, typename vecval>
    inline matrix<rowcount, colcount, vecval> matrix<rowcount, colcount, vecval>::operator +() const
    {
        return matrix<rowcount, colcount, vecval>(*this);
    }

    template <size_t rowcount, size_t colcount, typename vecval>
    inline matrix<rowcount, colcount, vecval> matrix<rowcount, colcount, vecval>::operator -() const
    {
        matrix<rowcount, colcount, vecval> result;

        for (size_t i = 0; i < colcount * rowcount; i++)
        {
            result.v[i] = -this->v[i];
        }

        return result;
    }

    template <size_t rowcount, size_t colcount, typename vecval>
    inline const vecval *matrix<rowcount, colcount, vecval>::operator [](size_t index) const
    {
        return this->m[index];
    }

    template <size_t rowcount, size_t colcount, typename vecval>
    inline vecval *matrix<rowcount, colcount, vecval>::operator [](size_t index)
    {
        return this->m[index];
    }

    template <size_t rowcount, size_t colcount, typename vecval>
    inline vector<colcount, vecval> matrix<rowcount, colcount, vecval>::get_row(size_t i) const
    {
        vector<colcount, vecval> result;

        for (size_t k = 0; k < colcount; k++)
        {
            result.v[k] = this->m[i][k];
        }

        return result;
    }

    template <size_t rowcount, size_t colcount, typename vecval>
    inline vector<rowcount, vecval> matrix<rowcount, colcount, vecval>::get_column(size_t j) const
    {
        vector<rowcount, vecval> result;

        for (size_t k = 0; k < rowcount; k++)
        {
            result.v[k] = this->m[k][j];
        }

        return result;
    }

    template <size_t rowcount, size_t colcount, typename vecval>
    void matrix<rowcount, colcount, vecval>::set_row(size_t i, const vector<colcount, vecval> &row)
    {
        for (size_t k = 0; k < colcount; k++)
        {
            this->m[i][k] = row[k];
        }
    }

    template <size_t rowcount, size_t colcount, typename vecval>
    void matrix<rowcount, colcount, vecval>::set_column(size_t j, const vector<rowcount, vecval> &column)
    {
        for (size_t k = 0; k < colcount; k++)
        {
            this->m[k][j] = column[k];
        }
    }

    template <typename vecval>
    constexpr matrix<2, 2, vecval>::matrix(std::initializer_list<vecval> values) : v()
    {
        assert(values.size() == 4);

        std::move(values.begin(), values.end(), this->v);
    }

    template <typename vecval>
    constexpr matrix<2, 2, vecval>::matrix(vecval value) : v()
    {
        this->m[0][1] = this->m[1][0] = traits<vecval>::zero;
        this->m[0][0] = this->m[1][1] = value;
    }

    template <typename vecval>
    constexpr matrix<2, 2, vecval>::matrix(const vecval *values) : v()
    {
        for (size_t i = 0; i < 4; i++)
        {
            this->v[i] = values[i];
        }
    }

    template <typename vecval>
    constexpr matrix<2, 2, vecval>::matrix(const matrix<3, 3, vecval> &mat) : v()
    {
        for (size_t i = 0; i < 4; i++)
        {
            this->v[i] = mat.v[i];
        }
    }

    template <typename vecval>
    constexpr matrix<2, 2, vecval>::matrix(const complex<vecval> &complex) : v()
    {
        this->v[0][0] = this->v[1][1] = complex.x;
        this->v[0][1] = complex.y;
        this->v[1][0] = -complex.y;
    }

    template <typename vecval>
    constexpr matrix<2, 2, vecval>::matrix(
        const vector<2, vecval> &row0,
        const vector<2, vecval> &row1
    ) : v()
    {
        this->m[0][0] = row0.x;
        this->m[0][1] = row0.y;
        this->m[1][0] = row1.x;
        this->m[1][1] = row1.y;
    }

    template <typename vecval>
    inline matrix<2, 2, vecval> &matrix<2, 2, vecval>::operator +=(const matrix<2, 2, vecval> &mat)
    {
        for (size_t i = 0; i < 4; i++)
        {
            this->v[i] += mat.v[i];
        }

        return *this;
    }

    template <typename vecval>
    inline matrix<2, 2, vecval> &matrix<2, 2, vecval>::operator -=(const matrix<2, 2, vecval> &mat)
    {
        for (size_t i = 0; i < 4; i++)
        {
            this->v[i] -= mat.v[i];
        }

        return *this;
    }

    template <typename vecval>
    inline matrix<2, 2, vecval> &matrix<2, 2, vecval>::operator *=(vecval d)
    {
        for (size_t i = 0; i < 4; i++)
        {
            this->v[i] *= d;
        }

        return *this;
    }

    template <typename vecval>
    inline matrix<2, 2, vecval> &matrix<2, 2, vecval>::operator /=(vecval d)
    {
        for (size_t i = 0; i < 4; i++)
        {
            this->v[i] /= d;
        }

        return *this;
    }

    template <typename vecval>
    inline matrix<2, 2, vecval> matrix<2, 2, vecval>::operator +() const
    {
        return matrix<2, 2, vecval>(*this);
    }

    template <typename vecval>
    inline matrix<2, 2, vecval> matrix<2, 2, vecval>::operator -() const
    {
        matrix<2, 2, vecval> result;

        for (size_t i = 0; i < 4; i++)
        {
            result.v[i] = -this->v[i];
        }

        return result;
    }

    template <typename vecval>
    inline vecval *matrix<2, 2, vecval>::operator [](size_t index)
    {
        return this->m[index];
    }

    template <typename vecval>
    inline const vecval *matrix<2, 2, vecval>::operator [](size_t index) const
    {
        return this->m[index];
    }

    template <typename vecval>
    inline vector<2, vecval> matrix<2, 2, vecval>::get_row(size_t i) const
    {
        return vector<2, vecval>(this->m[i][0], this->m[i][1]);
    }

    template <typename vecval>
    inline vector<2, vecval> matrix<2, 2, vecval>::get_column(size_t j) const
    {
        return vector<2, vecval>(this->m[0][j], this->m[1][j]);
    }

    template <typename vecval>
    constexpr matrix<3, 3, vecval>::matrix(std::initializer_list<vecval> values) : v()
    {
        assert(values.size() == 9);

        std::move(values.begin(), values.end(), this->v);
    }

    template <typename vecval>
    constexpr matrix<3, 3, vecval>::matrix(vecval value) : v()
    {
        this->m[0][1] = this->m[0][2] = this->m[1][0] = this->m[1][2] = this->m[2][0] = this->m[2][1] = traits<vecval>::zero;
        this->m[0][0] = this->m[1][1] = this->m[2][2] = value;
    }

    template <typename vecval>
    constexpr matrix<3, 3, vecval>::matrix(const vecval *values) : v()
    {
        this->v[0] = values[0];
        this->v[1] = values[1];
        this->v[2] = values[2];
        this->v[3] = values[3];
        this->v[4] = values[4];
        this->v[5] = values[5];
        this->v[6] = values[6];
        this->v[7] = values[7];
        this->v[8] = values[8];
    }

    template <typename vecval>
    constexpr matrix<3, 3, vecval>::matrix(const matrix<2, 2, vecval> &mat) : v()
    {
        this->m[0][0] = mat.m[0][0];
        this->m[0][1] = mat.m[0][1];
        this->m[1][0] = mat.m[1][0];
        this->m[1][1] = mat.m[1][1];

        this->m[0][2] = this->m[1][2] = this->m[2][0] = this->m[2][1] = traits<vecval>::zero;
        this->m[2][2] = traits<vecval>::unit;
    }

    template <typename vecval>
    constexpr matrix<3, 3, vecval>::matrix(const matrix<4, 4, vecval> &mat) : v()
    {
        for (size_t i = 0; i < 9; i++)
        {
            this->v[i] = mat.v[i];
        }
    }

    template <typename vecval>
    constexpr matrix<3, 3, vecval>::matrix(const quaternion<vecval> &quat) : v()
    {
        const auto qxx = quat.x * quat.x;
        const auto qyy = quat.y * quat.y;
        const auto qzz = quat.z * quat.z;
        const auto qxz = quat.x * quat.z;
        const auto qxy = quat.x * quat.y;
        const auto qyz = quat.y * quat.z;
        const auto qwx = quat.w * quat.x;
        const auto qwy = quat.w * quat.y;
        const auto qwz = quat.w * quat.z;

        this->m[0][0] = traits<vecval>::unit - vecval(traits<vecval>::type(2)) * (qyy + qzz);
        this->m[0][1] = vecval(2) * (qxy + qwz);
        this->m[0][2] = vecval(2) * (qxz - qwy);

        this->m[1][0] = vecval(2) * (qxy - qwz);
        this->m[1][1] = traits<vecval>::unit - vecval(traits<vecval>::type(2)) * (qxx + qzz);
        this->m[1][2] = vecval(2) * (qyz + qwx);

        this->m[2][0] = vecval(2) * (qxz + qwy);
        this->m[2][1] = vecval(2) * (qyz - qwx);
        this->m[2][2] = traits<vecval>::unit - vecval(traits<vecval>::type(2)) * (qxx + qyy);
    }

    template <typename vecval>
    constexpr matrix<3, 3, vecval>::matrix(
        const vector<3, vecval> &row0,
        const vector<3, vecval> &row1,
        const vector<3, vecval> &row2
    ) : v()
    {
        this->m[0][0] = row0.x;
        this->m[0][1] = row0.y;
        this->m[0][2] = row0.z;

        this->m[1][0] = row1.x;
        this->m[1][1] = row1.y;
        this->m[1][2] = row1.z;

        this->m[2][0] = row2.x;
        this->m[2][1] = row2.y;
        this->m[2][2] = row2.z;
    }

    template <typename vecval>
    inline auto matrix<3, 3, vecval>::operator +=(matrix<3, 3, vecval>::cref mat) -> matrix<3, 3, vecval>::ref
    {
        for (size_t i = 0; i < 9; i++)
        {
            this->v[i] += mat.v[i];
        }

        return *this;
    }

    template <typename vecval>
    inline auto matrix<3, 3, vecval>::operator -=(matrix<3, 3, vecval>::cref mat) -> matrix<3, 3, vecval>::ref
    {
        for (size_t i = 0; i < 9; i++)
        {
            this->v[i] -= mat.v[i];
        }

        return *this;
    }

    template <typename vecval>
    inline matrix<3, 3, vecval> &matrix<3, 3, vecval>::operator *=(vecval d)
    {
        for (size_t i = 0; i < 9; i++)
        {
            this->v[i] *= d;
        }

        return *this;
    }

    template <typename vecval>
    inline matrix<3, 3, vecval> &matrix<3, 3, vecval>::operator /=(vecval d)
    {
        for (size_t i = 0; i < 9; i++)
        {
            this->v[i] /= d;
        }

        return *this;
    }

    template <typename vecval>
    inline matrix<3, 3, vecval> matrix<3, 3, vecval>::operator +() const
    {
        return matrix<3, 3, vecval>(*this);
    }

    template <typename vecval>
    inline matrix<3, 3, vecval> matrix<3, 3, vecval>::operator -() const
    {
        matrix<3, 3, vecval> result;

        for (size_t i = 0; i < 9; i++)
        {
            result.v[i] = -this->v[i];
        }

        return result;
    }

    template <typename vecval>
    inline vecval *matrix<3, 3, vecval>::operator [](size_t index)
    {
        return this->m[index];
    }

    template <typename vecval>
    inline const vecval *matrix<3, 3, vecval>::operator [](size_t index) const
    {
        return this->m[index];
    }

    template <typename vecval>
    inline vector<3, vecval> matrix<3, 3, vecval>::get_row(size_t i) const
    {
        return vector<3, vecval>(this->m[i][0], this->m[i][1], this->m[i][2]);
    }

    template <typename vecval>
    inline vector<3, vecval> matrix<3, 3, vecval>::get_column(size_t j) const
    {
        return vector<3, vecval>(this->m[0][j], this->m[1][j], this->m[2][j]);
    }

    template <typename vecval>
    constexpr matrix<4, 4, vecval>::matrix(std::initializer_list<vecval> values) : v()
    {
        assert(values.size() == 16);

        std::move(values.begin(), values.end(), this->v);
    }

    template <typename vecval>
    constexpr matrix<4, 4, vecval>::matrix(vecval value) : v()
    {
        this->m[0][0] = value;
        this->m[0][1] = traits<vecval>::zero;
        this->m[0][2] = traits<vecval>::zero;
        this->m[0][3] = traits<vecval>::zero;

        this->m[1][0] = traits<vecval>::zero;
        this->m[1][1] = value;
        this->m[1][2] = traits<vecval>::zero;
        this->m[1][3] = traits<vecval>::zero;

        this->m[2][0] = traits<vecval>::zero;
        this->m[2][1] = traits<vecval>::zero;
        this->m[2][2] = value;
        this->m[2][3] = traits<vecval>::zero;

        this->m[3][0] = traits<vecval>::zero;
        this->m[3][1] = traits<vecval>::zero;
        this->m[3][2] = traits<vecval>::zero;
        this->m[3][3] = value;
    }

    template <typename vecval>
    constexpr matrix<4, 4, vecval>::matrix(const vecval *values) : v()
    {
        this->v[0] = values[0];
        this->v[1] = values[1];
        this->v[2] = values[2];
        this->v[3] = values[3];

        this->v[4] = values[4];
        this->v[5] = values[5];
        this->v[6] = values[6];
        this->v[7] = values[7];

        this->v[8] = values[8];
        this->v[9] = values[9];
        this->v[10] = values[10];
        this->v[11] = values[11];

        this->v[12] = values[12];
        this->v[13] = values[13];
        this->v[14] = values[14];
        this->v[15] = values[15];
    }

    template <typename vecval>
    constexpr matrix<4, 4, vecval>::matrix(const matrix<2, 2, vecval> &mat) : v()
    {
        this->m[0][0] = mat.m[0][0];
        this->m[0][1] = mat.m[0][1];
        this->m[0][2] = traits<vecval>::zero;
        this->m[0][3] = traits<vecval>::zero;

        this->m[1][0] = mat.m[1][0];
        this->m[1][1] = mat.m[1][1];
        this->m[1][2] = traits<vecval>::zero;
        this->m[1][3] = traits<vecval>::zero;

        this->m[2][0] = traits<vecval>::zero;
        this->m[2][1] = traits<vecval>::zero;
        this->m[2][2] = traits<vecval>::unit;
        this->m[2][3] = traits<vecval>::zero;

        this->m[3][0] = traits<vecval>::zero;
        this->m[3][1] = traits<vecval>::zero;
        this->m[3][2] = traits<vecval>::zero;
        this->m[3][3] = traits<vecval>::unit;
    }

    template <typename vecval>
    constexpr matrix<4, 4, vecval>::matrix(const matrix<3, 3, vecval> &mat) : v()
    {
        this->m[0][0] = mat.m[0][0];
        this->m[0][1] = mat.m[0][1];
        this->m[0][2] = mat.m[0][2];
        this->m[0][3] = traits<vecval>::zero;

        this->m[1][0] = mat.m[1][0];
        this->m[1][1] = mat.m[1][1];
        this->m[1][2] = mat.m[1][2];
        this->m[1][3] = traits<vecval>::zero;

        this->m[2][0] = mat.m[2][0];
        this->m[2][1] = mat.m[2][1];
        this->m[2][2] = mat.m[2][2];
        this->m[2][3] = traits<vecval>::zero;

        this->m[3][0] = traits<vecval>::zero;
        this->m[3][1] = traits<vecval>::zero;
        this->m[3][2] = traits<vecval>::zero;
        this->m[3][3] = traits<vecval>::unit;
    }

    template <typename vecval>
    constexpr matrix<4, 4, vecval>::matrix(const quaternion<vecval> &quat) : v()
    {
        const auto qxx = quat.x * quat.x;
        const auto qyy = quat.y * quat.y;
        const auto qzz = quat.z * quat.z;
        const auto qxz = quat.x * quat.z;
        const auto qxy = quat.x * quat.y;
        const auto qyz = quat.y * quat.z;
        const auto qwx = quat.w * quat.x;
        const auto qwy = quat.w * quat.y;
        const auto qwz = quat.w * quat.z;

        this->m[0][0] = traits<vecval>::unit - vecval(traits<vecval>::type(2)) * (qyy + qzz);
        this->m[0][1] = vecval(2) * (qxy + qwz);
        this->m[0][2] = vecval(2) * (qxz - qwy);
        this->m[0][3] = traits<vecval>::zero;

        this->m[1][0] = vecval(2) * (qxy - qwz);
        this->m[1][1] = traits<vecval>::unit - vecval(traits<vecval>::type(2)) * (qxx + qzz);
        this->m[1][2] = vecval(2) * (qyz + qwx);
        this->m[1][3] = traits<vecval>::zero;

        this->m[2][0] = vecval(2) * (qxz + qwy);
        this->m[2][1] = vecval(2) * (qyz - qwx);
        this->m[2][2] = traits<vecval>::unit - vecval(traits<vecval>::type(2)) * (qxx + qyy);
        this->m[2][3] = traits<vecval>::zero;

        this->m[3][0] = traits<vecval>::zero;
        this->m[3][1] = traits<vecval>::zero;
        this->m[3][2] = traits<vecval>::zero;
        this->m[3][3] = traits<vecval>::unit;
    }

    template <typename vecval>
    constexpr matrix<4, 4, vecval>::matrix(
        const vector<4, vecval> &row0,
        const vector<4, vecval> &row1,
        const vector<4, vecval> &row2,
        const vector<4, vecval> &row3
    ) : v()
    {
        this->m[0][0] = row0.x;
        this->m[0][1] = row0.y;
        this->m[0][2] = row0.z;
        this->m[0][3] = row0.w;

        this->m[1][0] = row1.x;
        this->m[1][1] = row1.y;
        this->m[1][2] = row1.z;
        this->m[1][3] = row1.w;

        this->m[2][0] = row2.x;
        this->m[2][1] = row2.y;
        this->m[2][2] = row2.z;
        this->m[2][3] = row2.w;

        this->m[3][0] = row3.x;
        this->m[3][1] = row3.y;
        this->m[3][2] = row3.z;
        this->m[3][3] = row3.w;
    }

    template <typename vecval>
    inline matrix<4, 4, vecval> &matrix<4, 4, vecval>::operator +=(const matrix<4, 4, vecval> &mat)
    {
        for (size_t i = 0; i < 16; i++)
        {
            this->v[i] += mat.v[i];
        }

        return *this;
    }

    template <typename vecval>
    inline matrix<4, 4, vecval> &matrix<4, 4, vecval>::operator -=(const matrix<4, 4, vecval> &mat)
    {
        for (size_t i = 0; i < 16; i++)
        {
            this->v[i] -= mat.v[i];
        }

        return *this;
    }

    template <typename vecval>
    inline matrix<4, 4, vecval> &matrix<4, 4, vecval>::operator *=(vecval d)
    {
        for (size_t i = 0; i < 16; i++)
        {
            this->v[i] *= d;
        }

        return *this;
    }

    template <typename vecval>
    inline matrix<4, 4, vecval> &matrix<4, 4, vecval>::operator /=(vecval d)
    {
        for (size_t i = 0; i < 16; i++)
        {
            this->v[i] /= d;
        }

        return *this;
    }

    template <typename vecval>
    inline matrix<4, 4, vecval> matrix<4, 4, vecval>::operator +() const
    {
        return matrix<4, 4, vecval>(*this);
    }

    template <typename vecval>
    inline matrix<4, 4, vecval> matrix<4, 4, vecval>::operator -() const
    {
        matrix<4, 4, vecval> result;

        for (size_t i = 0; i < 16; i++)
        {
            result.v[i] = -this->v[i];
        }

        return result;
    }

    template <typename vecval>
    inline vecval *matrix<4, 4, vecval>::operator [](size_t index)
    {
        return this->m[index];
    }

    template <typename vecval>
    inline const vecval *matrix<4, 4, vecval>::operator [](size_t index) const
    {
        return this->m[index];
    }

    template <typename vecval>
    inline vector<4, vecval> matrix<4, 4, vecval>::get_row(size_t i) const
    {
        return vector<4, vecval>(this->m[i][0], this->m[i][1], this->m[i][2], this->m[i][3]);
    }

    template <typename vecval>
    inline vector<4, vecval> matrix<4, 4, vecval>::get_column(size_t j) const
    {
        return vector<4, vecval>(this->m[0][j], this->m[1][j], this->m[2][j], this->m[3][j]);
    }

    template <size_t rowcount, size_t colcount, typename vecval>
    inline matrix<rowcount, colcount, vecval> operator +(const matrix<rowcount, colcount, vecval> &a, const matrix<rowcount, colcount, vecval> &b)
    {
        return add(a, b);
    }

    template <size_t rowcount, size_t colcount, typename vecval>
    inline matrix<rowcount, colcount, vecval> operator -(const matrix<rowcount, colcount, vecval> &a, const matrix<rowcount, colcount, vecval> &b)
    {
        return subtract(a, b);
    }

    template <size_t rowcount, size_t colcount, typename vecval>
    inline matrix<rowcount, colcount, vecval> operator *(const matrix<rowcount, colcount, vecval> &mat, vecval d)
    {
        matrix<rowcount, colcount, vecval> temp;

        for (size_t i = 0; i < colcount * rowcount; i++)
        {
            temp.v[i] = mat.v[i] * d;
        }

        return temp;
    }

    template <size_t rowcount, size_t colcount, typename vecval>
    inline matrix<rowcount, colcount, vecval> operator /(const matrix<rowcount, colcount, vecval> &mat, vecval d)
    {
        matrix<rowcount, colcount, vecval> temp;

        for (size_t i = 0; i < colcount * rowcount; i++)
        {
            temp.v[i] = mat.v[i] / d;
        }

        return temp;
    }

    template <size_t a_rows, size_t a_columns, size_t b_columns, typename vecval>
    inline matrix<a_rows, b_columns, vecval> operator *(const matrix<a_rows, a_columns, vecval> &a, const matrix<a_columns, b_columns, vecval> &b)
    {
        matrix<a_rows, b_columns, vecval> result;

        for (size_t i = 0; i < a_rows; i++)
        {
            for (size_t j = 0; j < b_columns; j++)
            {
                result[i][j] = traits<vecval>::zero;

                for (size_t k = 0; k < a_columns; k++)
                {
                    result[i][j] += a[i][k] * b[k][j];
                }
            }
        }

        return result;
    }

    template <size_t rowcount, size_t colcount, typename vecval>
    inline vector<rowcount, vecval> operator *(const matrix<rowcount, colcount, vecval> &mat, const vector<colcount, vecval> &vec)
    {
        vector<rowcount, vecval> result;

        for (size_t i = 0; i < rowcount; i++)
        {
            result[i] = traits<vecval>::zero;

            for (size_t j = 0; j < colcount; j++)
            {
                result[i] += mat[i][j] * vec[j];
            }
        }

        return result;
    }

    template <size_t rowcount, size_t colcount, typename vecval>
    inline vector<colcount, vecval> operator *(const vector<rowcount, vecval> &vec, const matrix<rowcount, colcount, vecval> &mat)
    {
        vector<colcount, vecval> result;

        for (size_t i = 0; i < colcount; i++)
        {
            result[i] = traits<vecval>::zero;

            for (size_t j = 0; j < rowcount; j++)
            {
                result[i] += vec[j] * mat[j][i];
            }
        }

        return result;
    }

    template <size_t rowcount, size_t colcount, typename vecval>
    inline auto operator ==(const matrix<rowcount, colcount, vecval> &a, const matrix<rowcount, colcount, vecval> &b) -> bool
    {
        return equals(a, b);
    }

    template <size_t rowcount, size_t colcount, typename vecval>
    inline auto operator !=(const matrix<rowcount, colcount, vecval> &a, const matrix<rowcount, colcount, vecval> &b) -> bool
    {
        return !equals(a, b);
    }

    template <size_t rowcount, size_t colcount, typename vecval>
    inline matrix<rowcount, colcount, vecval> add(const matrix<rowcount, colcount, vecval> &a, const matrix<rowcount, colcount, vecval> &b)
    {
        matrix<rowcount, colcount, vecval> result;

        for (size_t i = 0; i < colcount * rowcount; i++)
        {
            result.v[i] = a.v[i] + b.v[i];
        }

        return result;
    }

    template <size_t rowcount, size_t colcount, typename vecval>
    inline matrix<rowcount, colcount, vecval> subtract(const matrix<rowcount, colcount, vecval> &a, const matrix<rowcount, colcount, vecval> &b)
    {
        matrix<rowcount, colcount, vecval> result;

        for (size_t i = 0; i < colcount * rowcount; i++)
        {
            result.v[i] = a.v[i] - b.v[i];
        }

        return result;
    }

    template <size_t colcount, typename vecval>
    inline vecval trace(const matrix<colcount, colcount, vecval> &mat)
    {
        vecval result = traits<vecval>::zero;

        for (size_t i = 0; i < colcount; i++)
        {
            result += mat[i][i];
        }

        return result;
    }

    template <typename vecval>
    inline vecval det(const matrix<2, 2, vecval> &mat)
    {
        return
            mat.m[0][0] * mat.m[1][1] -
            mat.m[1][0] * mat.m[0][1];
    }

    template <typename vecval>
    inline vecval det(const matrix<3, 3, vecval> &mat)
    {
        return
            mat.m[0][0] * (mat.m[1][1] * mat.m[2][2] - mat.m[2][1] * mat.m[1][2]) -
            mat.m[1][0] * (mat.m[0][1] * mat.m[2][2] - mat.m[2][1] * mat.m[0][2]) +
            mat.m[2][0] * (mat.m[0][1] * mat.m[1][2] - mat.m[1][1] * mat.m[0][2]);
    }

    template <typename vecval>
    inline vecval det(const matrix<4, 4, vecval> &mat)
    {
        const auto s0 = mat.m[2][2] * mat.m[3][3] - mat.m[3][2] * mat.m[2][3];
        const auto s1 = mat.m[2][1] * mat.m[3][3] - mat.m[3][1] * mat.m[2][3];
        const auto s2 = mat.m[2][1] * mat.m[3][2] - mat.m[3][1] * mat.m[2][2];
        const auto s3 = mat.m[2][0] * mat.m[3][3] - mat.m[3][0] * mat.m[2][3];
        const auto s4 = mat.m[2][0] * mat.m[3][2] - mat.m[3][0] * mat.m[2][2];
        const auto s5 = mat.m[2][0] * mat.m[3][1] - mat.m[3][0] * mat.m[2][1];

        const auto d0 = +(mat.m[1][1] * s0 - mat.m[1][2] * s1 + mat.m[1][3] * s2);
        const auto d1 = -(mat.m[1][0] * s0 - mat.m[1][2] * s3 + mat.m[1][3] * s4);
        const auto d2 = +(mat.m[1][0] * s1 - mat.m[1][1] * s3 + mat.m[1][3] * s5);
        const auto d3 = -(mat.m[1][0] * s2 - mat.m[1][1] * s4 + mat.m[1][2] * s5);

        return
            mat.m[0][0] * d0 + mat.m[0][1] * d1 +
            mat.m[0][2] * d2 + mat.m[0][3] * d3;
    }

    template <typename vecval>
    inline matrix<2, 2, vecval> inverse(const matrix<2, 2, vecval> &mat)
    {
        const auto d = det(mat);

        return {
            +mat.m[1][1] / d,
            -mat.m[0][1] / d,
            -mat.m[1][0] / d,
            +mat.m[0][0] / d
        };
    }

    template <typename vecval>
    inline matrix<3, 3, vecval> inverse(const matrix<3, 3, vecval> &mat)
    {
        const auto d = det(mat);

        return {
            +(mat.m[1][1] * mat.m[2][2] - mat.m[2][1] * mat.m[1][2]) / d,
            -(mat.m[0][1] * mat.m[2][2] - mat.m[2][1] * mat.m[0][2]) / d,
            +(mat.m[0][1] * mat.m[1][2] - mat.m[1][1] * mat.m[0][2]) / d,
            -(mat.m[1][0] * mat.m[2][2] - mat.m[2][0] * mat.m[1][2]) / d,
            +(mat.m[0][0] * mat.m[2][2] - mat.m[2][0] * mat.m[0][2]) / d,
            +(mat.m[1][0] * mat.m[2][1] - mat.m[2][0] * mat.m[1][1]) / d,
            -(mat.m[0][0] * mat.m[1][2] - mat.m[1][0] * mat.m[0][2]) / d,
            -(mat.m[0][0] * mat.m[2][1] - mat.m[2][0] * mat.m[0][1]) / d,
            +(mat.m[0][0] * mat.m[1][1] - mat.m[1][0] * mat.m[0][1]) / d
        };
    }

    template <typename vecval>
    inline matrix<4, 4, vecval> inverse(const matrix<4, 4, vecval> &mat)
    {
        const auto c00 = mat.m[2][2] * mat.m[3][3] - mat.m[3][2] * mat.m[2][3];
        const auto c02 = mat.m[1][2] * mat.m[3][3] - mat.m[3][2] * mat.m[1][3];
        const auto c03 = mat.m[1][2] * mat.m[2][3] - mat.m[2][2] * mat.m[1][3];

        const auto c04 = mat.m[2][1] * mat.m[3][3] - mat.m[3][1] * mat.m[2][3];
        const auto c06 = mat.m[1][1] * mat.m[3][3] - mat.m[3][1] * mat.m[1][3];
        const auto c07 = mat.m[1][1] * mat.m[2][3] - mat.m[2][1] * mat.m[1][3];

        const auto c08 = mat.m[2][1] * mat.m[3][2] - mat.m[3][1] * mat.m[2][2];
        const auto c10 = mat.m[1][1] * mat.m[3][2] - mat.m[3][1] * mat.m[1][2];
        const auto c11 = mat.m[1][1] * mat.m[2][2] - mat.m[2][1] * mat.m[1][2];

        const auto c12 = mat.m[2][0] * mat.m[3][3] - mat.m[3][0] * mat.m[2][3];
        const auto c14 = mat.m[1][0] * mat.m[3][3] - mat.m[3][0] * mat.m[1][3];
        const auto c15 = mat.m[1][0] * mat.m[2][3] - mat.m[2][0] * mat.m[1][3];

        const auto c16 = mat.m[2][0] * mat.m[3][2] - mat.m[3][0] * mat.m[2][2];
        const auto c18 = mat.m[1][0] * mat.m[3][2] - mat.m[3][0] * mat.m[1][2];
        const auto c19 = mat.m[1][0] * mat.m[2][2] - mat.m[2][0] * mat.m[1][2];

        const auto c20 = mat.m[2][0] * mat.m[3][1] - mat.m[3][0] * mat.m[2][1];
        const auto c22 = mat.m[1][0] * mat.m[3][1] - mat.m[3][0] * mat.m[1][1];
        const auto c23 = mat.m[1][0] * mat.m[2][1] - mat.m[2][0] * mat.m[1][1];

        const vector<4, vecval> f0(c00, c00, c02, c03);
        const vector<4, vecval> f1(c04, c04, c06, c07);
        const vector<4, vecval> f2(c08, c08, c10, c11);
        const vector<4, vecval> f3(c12, c12, c14, c15);
        const vector<4, vecval> f4(c16, c16, c18, c19);
        const vector<4, vecval> f5(c20, c20, c22, c23);

        const vector<4, vecval> v0(mat.m[1][0], mat.m[0][0], mat.m[0][0], mat.m[0][0]);
        const vector<4, vecval> v1(mat.m[1][1], mat.m[0][1], mat.m[0][1], mat.m[0][1]);
        const vector<4, vecval> v2(mat.m[1][2], mat.m[0][2], mat.m[0][2], mat.m[0][2]);
        const vector<4, vecval> v3(mat.m[1][3], mat.m[0][3], mat.m[0][3], mat.m[0][3]);

        const vector<4, vecval> inv0(v1 * f0 - v2 * f1 + v3 * f2);
        const vector<4, vecval> inv1(v0 * f0 - v2 * f3 + v3 * f4);
        const vector<4, vecval> inv2(v0 * f1 - v1 * f3 + v3 * f5);
        const vector<4, vecval> inv3(v0 * f2 - v1 * f4 + v2 * f5);

        const vector<4, vecval> sa(+1, -1, +1, -1);
        const vector<4, vecval> sb(-1, +1, -1, +1);

        const matrix<4, 4, vecval> inv(inv0 * sa, inv1 * sb, inv2 * sa, inv3 * sb);

        const vector<4, vecval> row0(inv[0][0], inv[1][0], inv[2][0], inv[3][0]);
        const vector<4, vecval> dot0(mat.m[0] * row0);

        const auto dot1 = (dot0.x + dot0.y) + (dot0.z + dot0.w);

        return inv / dot1;
    }

    template <size_t rowcount, size_t colcount, typename vecval>
    inline matrix<rowcount, colcount, vecval> transpose(const matrix<rowcount, colcount, vecval> &mat)
    {
        matrix<rowcount, colcount, vecval> result;

        for (size_t i = 0; i < rowcount; i++)
        {
            for (size_t j = 0; j < colcount; j++)
            {
                result.m[i][j] = mat.m[j][i];
            }
        }

        return result;
    }

    template <typename vecval>
    inline auto normalize_rotation(const matrix<3, 3, vecval> &mat) -> matrix<3, 3, vecval>
    {
        vector<3, vecval> x_axis = { mat[0][0], mat[1][0], mat[2][0] };
        vector<3, vecval> y_axis = { mat[0][1], mat[1][1], mat[2][1] };
        vector<3, vecval> z_axis = cross(x_axis, y_axis);

        normalize(x_axis);
        normalize(y_axis);
        normalize(z_axis);

        return {
            x_axis.x, y_axis.x, z_axis.x,
            x_axis.y, y_axis.y, z_axis.y,
            x_axis.z, y_axis.z, z_axis.z,
        };
    }
}

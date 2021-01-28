namespace laplace::math
{
    template <size_t valcount, typename vecval>
    constexpr vector<valcount, vecval>::vector(std::initializer_list<vecval> values) : v()
    {
        assert(values.size() == valcount);

        for (size_t i = 0; i < valcount; i++)
        {
            this->v[i] = values.begin()[i];
        }
    }

    template <size_t valcount, typename vecval>
    constexpr vector<valcount, vecval>::vector(vecval value) : v()
    {
        for (size_t i = 0; i < valcount; i++)
        {
            this->v[i] = value;
        }
    }

    template <size_t valcount, typename vecval>
    constexpr vector<valcount, vecval>::vector(const vecval *values) : v()
    {
        for (size_t i = 0; i < valcount; i++)
        {
            this->v[i] = values[i];
        }
    }

    template <size_t valcount, typename vecval>
    inline vector<valcount, vecval> &vector<valcount, vecval>::operator +=(const vector<valcount, vecval> &v)
    {
        for (size_t i = 0; i < valcount; i++)
        {
            this->v[i] += v.v[i];
        }

        return *this;
    }

    template <size_t valcount, typename vecval>
    inline vector<valcount, vecval> &vector<valcount, vecval>::operator -=(const vector<valcount, vecval> &v)
    {
        for (size_t i = 0; i < valcount; i++)
        {
            this->v[i] -= v.v[i];
        }

        return *this;
    }

    template <size_t valcount, typename vecval>
    inline vector<valcount, vecval> &vector<valcount, vecval>::operator *=(vecval d)
    {
        for (size_t i = 0; i < valcount; i++)
        {
            this->v[i] *= d;
        }

        return *this;
    }

    template <size_t valcount, typename vecval>
    inline vector<valcount, vecval> &vector<valcount, vecval>::operator /=(vecval d)
    {
        for (size_t i = 0; i < valcount; i++)
        {
            this->v[i] /= d;
        }

        return *this;
    }

    template <size_t valcount, typename vecval>
    inline vector<valcount, vecval> &vector<valcount, vecval>::operator *=(const vector<valcount, vecval> &v)
    {
        for (size_t i = 0; i < valcount; i++)
        {
            this->v[i] *= v.v[i];
        }

        return *this;
    }

    template <size_t valcount, typename vecval>
    inline vector<valcount, vecval> &vector<valcount, vecval>::operator /=(const vector<valcount, vecval> &v)
    {
        for (size_t i = 0; i < valcount; i++)
        {
            this->v[i] /= v.v[i];
        }

        return *this;
    }

    template <size_t valcount, typename vecval>
    inline vector<valcount, vecval> vector<valcount, vecval>::operator +() const
    {
        return vector<valcount, vecval>(*this);
    }

    template <size_t valcount, typename vecval>
    inline vector<valcount, vecval> vector<valcount, vecval>::operator -() const
    {
        vector<valcount, vecval> temp;

        for (size_t i = 0; i < valcount; i++)
        {
            temp.v[i] = -this->v[i];
        }

        return temp;
    }

    template <size_t valcount, typename vecval>
    inline vecval vector<valcount, vecval>::operator [](size_t index) const
    {
        return this->v[index];
    }

    template <size_t valcount, typename vecval>
    inline vecval &vector<valcount, vecval>::operator [](size_t index)
    {
        return this->v[index];
    }

    template <typename vecval>
    constexpr vector<2, vecval>::vector(vecval x0, vecval y0) :
        x(x0), y(y0)
    {
    }

    template <typename vecval>
    constexpr vector<2, vecval>::vector(std::initializer_list<vecval> values) : v()
    {
        assert(values.size() == 2);

        this->x = values.begin()[0];
        this->y = values.begin()[1];
    }

    template <typename vecval>
    constexpr vector<2, vecval>::vector(vecval value) :
        x(value), y(value)
    {
    }

    template <typename vecval>
    constexpr vector<2, vecval>::vector(const vecval *v) :
        x(v[0]),
        y(v[1])
    {
    }

    template <typename vecval>
    constexpr vector<2, vecval>::vector(const complex<vecval> &c) :
        x(c.x),
        y(c.y)
    {
    }

    template <typename vecval>
    constexpr vector<2, vecval>::vector(const vector<3, vecval> &v) :
        x(v.x),
        y(v.y)
    {
    }

    template <typename vecval>
    constexpr vector<2, vecval>::vector(const vector<4, vecval> &v) :
        x(v.x),
        y(v.y)
    {
    }

    template <typename vecval>
    inline vector<2, vecval> &vector<2, vecval>::operator +=(const vector<2, vecval> &v)
    {
        this->x += v.x;
        this->y += v.y;

        return *this;
    }

    template <typename vecval>
    inline vector<2, vecval> &vector<2, vecval>::operator -=(const vector<2, vecval> &v)
    {
        this->x -= v.x;
        this->y -= v.y;

        return *this;
    }

    template <typename vecval>
    inline vector<2, vecval> &vector<2, vecval>::operator *=(vecval d)
    {
        this->x *= d;
        this->y *= d;

        return *this;
    }

    template <typename vecval>
    inline vector<2, vecval> &vector<2, vecval>::operator /=(vecval v)
    {
        this->x /= v;
        this->y /= v;

        return *this;
    }

    template <typename vecval>
    inline vector<2, vecval> &vector<2, vecval>::operator *=(const vector<2, vecval> &v)
    {
        this->x *= v.x;
        this->y *= v.y;

        return *this;
    }

    template <typename vecval>
    inline vector<2, vecval> &vector<2, vecval>::operator /=(const vector<2, vecval> &v)
    {
        this->x /= v.x;
        this->y /= v.y;

        return *this;
    }

    template <typename vecval>
    inline vector<2, vecval> vector<2, vecval>::operator +() const
    {
        return vector<2, vecval>(*this);
    }

    template <typename vecval>
    inline vector<2, vecval> vector<2, vecval>::operator -() const
    {
        return vector<2, vecval>(-this->x, -this->y);
    }

    template <typename vecval>
    inline vecval vector<2, vecval>::operator [](size_t index) const
    {
        return this->v[index];
    }

    template <typename vecval>
    inline vecval &vector<2, vecval>::operator [](size_t index)
    {
        return this->v[index];
    }

    template <typename vecval>
    constexpr vector<3, vecval>::vector(vecval x0, vecval y0, vecval z0) :
        x(x0),
        y(y0),
        z(z0)
    {
    }

    template <typename vecval>
    constexpr vector<3, vecval>::vector(std::initializer_list<vecval> values) : v()
    {
        assert(values.size() == 3);

        this->x = values.begin()[0];
        this->y = values.begin()[1];
        this->z = values.begin()[2];
    }

    template <typename vecval>
    constexpr vector<3, vecval>::vector(vecval value) :
        x(value), y(value), z(value)
    {
    }

    template <typename vecval>
    constexpr vector<3, vecval>::vector(const vecval *values) :
        x(values[0]),
        y(values[1]),
        z(values[2])
    {
    }

    template <typename vecval>
    constexpr vector<3, vecval>::vector(const vector<2, vecval> &vec, vecval z0) :
        x(vec.x),
        y(vec.y),
        z(z0)
    {
    }

    template <typename vecval>
    constexpr vector<3, vecval>::vector(const vector<4, vecval> &vec) :
        x(vec.x),
        y(vec.y),
        z(vec.z)
    {
    }

    template <typename vecval>
    inline vector<3, vecval> &vector<3, vecval>::operator +=(const vector<3, vecval> &v)
    {
        this->x += v.x;
        this->y += v.y;
        this->z += v.z;

        return *this;
    }

    template <typename vecval>
    inline vector<3, vecval> &vector<3, vecval>::operator -=(const vector<3, vecval> &v)
    {
        this->x -= v.x;
        this->y -= v.y;
        this->z -= v.z;

        return *this;
    }

    template <typename vecval>
    inline vector<3, vecval> &vector<3, vecval>::operator *=(vecval d)
    {
        this->x *= d;
        this->y *= d;
        this->z *= d;

        return *this;
    }

    template <typename vecval>
    inline vector<3, vecval> &vector<3, vecval>::operator /=(vecval v)
    {
        this->x /= v;
        this->y /= v;
        this->z /= v;

        return *this;
    }

    template <typename vecval>
    inline vector<3, vecval> &vector<3, vecval>::operator *=(const vector<3, vecval> &v)
    {
        this->x *= v.x;
        this->y *= v.y;
        this->z *= v.z;

        return *this;
    }

    template <typename vecval>
    inline vector<3, vecval> &vector<3, vecval>::operator /=(const vector<3, vecval> &v)
    {
        this->x /= v.x;
        this->y /= v.y;
        this->z /= v.z;

        return *this;
    }

    template <typename vecval>
    inline vector<3, vecval> vector<3, vecval>::operator +() const
    {
        return vector<3, vecval>(*this);
    }

    template <typename vecval>
    inline vector<3, vecval> vector<3, vecval>::operator -() const
    {
        return vector<3, vecval>(-this->x, -this->y, -this->z);
    }

    template <typename vecval>
    inline vecval vector<3, vecval>::operator [](size_t index) const
    {
        return this->v[index];
    }

    template <typename vecval>
    inline vecval &vector<3, vecval>::operator [](size_t index)
    {
        return this->v[index];
    }

    template <typename vecval>
    constexpr vector<4, vecval>::vector(vecval x0, vecval y0, vecval z0, vecval w0) :
        x(x0),
        y(y0),
        z(z0),
        w(w0)
    {
    }

    template <typename vecval>
    constexpr vector<4, vecval>::vector(std::initializer_list<vecval> values) : v()
    {
        assert(values.size() == 4);

        this->x = values.begin()[0];
        this->y = values.begin()[1];
        this->z = values.begin()[2];
        this->w = values.begin()[3];
    }

    template <typename vecval>
    constexpr vector<4, vecval>::vector(vecval value) :
        x(value),
        y(value),
        z(value),
        w(value)
    {
    }

    template <typename vecval>
    constexpr vector<4, vecval>::vector(const vecval *values) :
        x(values[0]),
        y(values[1]),
        z(values[2]),
        w(values[3])
    {
    }

    template <typename vecval>
    constexpr vector<4, vecval>::vector(const vector<2, vecval> &vec, vecval z0, vecval w0) :
        x(vec.x),
        y(vec.y),
        z(z0),
        w(w0)
    {
    }

    template <typename vecval>
    constexpr vector<4, vecval>::vector(const vector<3, vecval> &vec, vecval w0) :
        x(vec.x),
        y(vec.y),
        z(vec.z),
        w(w0)
    {
    }

    template <typename vecval>
    constexpr vector<4, vecval>::vector(const quaternion<vecval> &quat) :
        x(quat.x),
        y(quat.y),
        z(quat.z),
        w(quat.w)
    {
    }

    template <typename vecval>
    inline vector<4, vecval> &vector<4, vecval>::operator +=(const vector<4, vecval> &v)
    {
        this->x += v.x;
        this->y += v.y;
        this->z += v.z;
        this->w += v.w;

        return *this;
    }

    template <typename vecval>
    inline vector<4, vecval> &vector<4, vecval>::operator -=(const vector<4, vecval> &v)
    {
        this->x -= v.x;
        this->y -= v.y;
        this->z -= v.z;
        this->w -= v.w;

        return *this;
    }

    template <typename vecval>
    inline vector<4, vecval> &vector<4, vecval>::operator *=(vecval d)
    {
        this->x *= d;
        this->y *= d;
        this->z *= d;
        this->w *= d;

        return *this;
    }

    template <typename vecval>
    inline vector<4, vecval> &vector<4, vecval>::operator /=(vecval v)
    {
        this->x /= v;
        this->y /= v;
        this->z /= v;
        this->w /= v;

        return *this;
    }

    template <typename vecval>
    inline vector<4, vecval> &vector<4, vecval>::operator *=(const vector<4, vecval> &v)
    {
        this->x *= v.x;
        this->y *= v.y;
        this->z *= v.z;
        this->w *= v.w;

        return *this;
    }

    template <typename vecval>
    inline vector<4, vecval> &vector<4, vecval>::operator /=(const vector<4, vecval> &v)
    {
        this->x /= v.x;
        this->y /= v.y;
        this->z /= v.z;
        this->w /= v.w;

        return *this;
    }

    template <typename vecval>
    inline vector<4, vecval> vector<4, vecval>::operator +() const
    {
        return vector<4, vecval>(*this);
    }

    template <typename vecval>
    inline vector<4, vecval> vector<4, vecval>::operator -() const
    {
        return vector<4, vecval>(-this->x, -this->y, -this->z, -this->w);
    }

    template <typename vecval>
    inline vecval vector<4, vecval>::operator [](size_t index) const
    {
        return this->v[index];
    }

    template <typename vecval>
    inline vecval &vector<4, vecval>::operator [](size_t index)
    {
        return this->v[index];
    }

    template <size_t valcount, typename vecval>
    inline vector<valcount, vecval> operator +(const vector<valcount, vecval> &u, const vector<valcount, vecval> &v)
    {
        vector<valcount, vecval> result;

        for (size_t i = 0; i < valcount; i++)
        {
            result.v[i] = u.v[i] + v.v[i];
        }

        return result;
    }

    template <size_t valcount, typename vecval>
    inline vector<valcount, vecval> operator -(const vector<valcount, vecval> &u, const vector<valcount, vecval> &v)
    {
        vector<valcount, vecval> result;

        for (size_t i = 0; i < valcount; i++)
        {
            result[i] = u[i] - v[i];
        }

        return result;
    }

    template <size_t valcount, typename vecval>
    inline vector<valcount, vecval> operator *(const vector<valcount, vecval> &v, vecval d)
    {
        vector<valcount, vecval> result;

        for (size_t i = 0; i < valcount; i++)
        {
            result[i] = v[i] * d;
        }

        return result;
    }

    template <size_t valcount, typename vecval>
    inline vector<valcount, vecval> operator /(const vector<valcount, vecval> &v, vecval d)
    {
        vector<valcount, vecval> result;

        for (size_t i = 0; i < valcount; i++)
        {
            result.v[i] = v.v[i] / d;
        }

        return result;
    }

    template <size_t valcount, typename vecval>
    inline vector<valcount, vecval> operator *(const vector<valcount, vecval> &u, const vector<valcount, vecval> &v)
    {
        vector<valcount, vecval> result;

        for (size_t i = 0; i < valcount; i++)
        {
            result[i] = u[i] * v[i];
        }

        return result;
    }

    template <size_t valcount, typename vecval>
    inline vector<valcount, vecval> operator /(const vector<valcount, vecval> &u, const vector<valcount, vecval> &v)
    {
        vector<valcount, vecval> result;

        for (size_t i = 0; i < valcount; i++)
        {
            result[i] = u[i] / v[i];
        }

        return result;
    }

    template <size_t valcount, typename vecval>
    inline vector<valcount, vecval> operator *(vecval d, const vector<valcount, vecval> &v)
    {
        return v * d;
    }

    template <size_t valcount, typename vecval>
    inline auto operator ==(const vector<valcount, vecval> &a, const vector<valcount, vecval> &b) -> bool
    {
        return equals(a, b);
    }

    template <size_t valcount, typename vecval>
    inline auto operator !=(const vector<valcount, vecval> &a, const vector<valcount, vecval> &b) -> bool
    {
        return !equals(a, b);
    }

    template <size_t valcount, typename vecval>
    inline vecval square_length(const vector<valcount, vecval> &v)
    {
        vecval sqlen = traits<vecval>::zero;

        for (size_t i = 0; i < valcount; i++)
        {
            sqlen += v.v[i] * v.v[i];
        }

        return sqlen;
    }

    template <size_t valcount, typename vecval>
    inline vecval length(const vector<valcount, vecval> &v)
    {
        return vecval(sqrt(static_cast<double>(square_length(v))));
    }

    template <size_t valcount>
    inline realmax_t length(const vector<valcount, realmax_t> &v)
    {
        return sqrtl(square_length(v));
    }

    template <size_t valcount>
    inline int64_t length(const vector<valcount, int64_t> &v)
    {
        return int64_t(sqrtl(static_cast<realmax_t>(square_length(v))));
    }

    template <size_t valcount, typename vecval>
    inline vecval dot(const vector<valcount, vecval> &u, const vector<valcount, vecval> &v)
    {
        vecval d = vecval();

        for (size_t i = 0; i < valcount; i++)
        {
            d += u[i] * v[i];
        }

        return d;
    }

    template <size_t valcount, typename vecval>
    inline vector<valcount, vecval> normal(const vector<valcount, vecval> &v)
    {
        return v / length(v);
    }

    template <size_t valcount, typename vecval>
    inline vector<valcount, vecval> &normalize(vector<valcount, vecval> &v)
    {
        return v /= length(v);
    }

    template <typename vecval>
    inline vector<3, vecval> cross(const vector<3, vecval> &u, const vector<3, vecval> &v)
    {
        vector<3, vecval> result;
        result.x = u.y * v.z - u.z * v.y;
        result.y = u.z * v.x - u.x * v.z;
        result.z = u.x * v.y - u.y * v.x;
        return result;
    }

    template <size_t valcount, typename vecval>
    vector<valcount, vecval> project(const vector<valcount, vecval> &vec, const vector<valcount, vecval> norm)
    {
        return vec - dot(vec, norm) * norm;
    }

    template <typename vecval>
    inline vector<2, vecval> rotate(const vector<2, vecval> &v, const complex<vecval> &a)
    {
        complex<vecval> result = complex<vecval>(v) * a;
        return vector<2, vecval>(result.x, result.y);
    }

    template <typename vecval>
    inline vector<3, vecval> rotate(const vector<3, vecval> &v, const quaternion<vecval> &q)
    {
        return vector<3, vecval>((q * quaternion<vecval>(v, vecval(0.0)) * conjugation(q)).v);
    }

    template <typename vecval>
    inline vector<4, vecval> rotate(const vector<4, vecval> &v, const quaternion<vecval> &q)
    {
        return vector<4, vecval>(q * quaternion<vecval>(v) * conjugation(q));
    }
}

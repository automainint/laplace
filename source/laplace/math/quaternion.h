#pragma once

#include "traits.h"
#include <limits>
#include <initializer_list>
#include <cmath>
#include <cassert>

namespace laplace::math
{
	/*	Quaternion.
	 *
	 *	Q = x i + y j + z k + w,
	 *	where i^2 = j^2 = k^2 = -1.
	 */
    template <typename vecval>
    class quaternion
    {
    public:
        typedef vecval type;

		static constexpr size_t size = 4;

        union {
            struct {
                vecval x, y, z, w;
            };
            struct {
                vecval v[4];
            };
        };

        constexpr quaternion() = default;
		constexpr quaternion(vecval x, vecval y, vecval z, vecval w);
		constexpr quaternion(std::initializer_list<vecval> values);

        explicit constexpr quaternion(vecval d);
        explicit constexpr quaternion(const vecval *v);
        explicit constexpr quaternion(const vector<3, vecval> &vec, vecval w);
        explicit constexpr quaternion(const vector<4, vecval> &vec);
        explicit constexpr quaternion(const matrix<3, 3, vecval> &mat);
        explicit constexpr quaternion(const matrix<4, 4, vecval> &mat);

        quaternion<vecval> &operator +=(const quaternion<vecval> &quat);
        quaternion<vecval> &operator -=(const quaternion<vecval> &quat);
        quaternion<vecval> &operator *=(vecval d);
        quaternion<vecval> &operator /=(vecval d);

        quaternion<vecval> operator +() const;
        quaternion<vecval> operator -() const;

        bool operator ==(const quaternion<vecval> &quat) const;
        bool operator !=(const quaternion<vecval> &quat) const;

        vecval operator [](size_t index) const;
        vecval &operator [](size_t index);

		template <typename new_vecval>
		auto to() const -> quaternion<new_vecval>;
    };

    template <typename vecval>
    quaternion<vecval> operator +(const quaternion<vecval> &q, const quaternion<vecval> &p);

    template <typename vecval>
    quaternion<vecval> operator -(const quaternion<vecval> &q, const quaternion<vecval> &p);

    template <typename vecval>
    quaternion<vecval> operator *(const quaternion<vecval> &q, const quaternion<vecval> &p);

    template <typename vecval>
    quaternion<vecval> operator *(const quaternion<vecval> &q, vecval d);

    template <typename vecval>
    quaternion<vecval> operator /(const quaternion<vecval> &q, vecval d);

    template <typename vecval>
    vecval square_length(const quaternion<vecval> &q);

    template <typename vecval>
    vecval length(const quaternion<vecval> &q);

    template <typename vecval>
    quaternion<vecval> normal(const quaternion<vecval> &q);

    template <typename vecval>
    quaternion<vecval> conjugation(const quaternion<vecval> &q);

    template <typename vecval>
    quaternion<vecval> inverse(const quaternion<vecval> &q);

    template <typename vecval>
    quaternion<vecval> &normalize(quaternion<vecval> &q);

    template <typename vecval>
    quaternion<vecval> &conjugate(quaternion<vecval> &q);

    template <typename vecval>
    quaternion<vecval> &invert(quaternion<vecval> &q);

    template <typename vecval>
    vecval inner(const quaternion<vecval> &q, const quaternion<vecval> &p);

    template <typename vecval>
    quaternion<vecval> slerp(const quaternion<vecval> &q, const quaternion<vecval> &p, realmax_t t);

    template <typename vecval>
    constexpr quaternion<vecval>::quaternion(vecval x0, vecval y0, vecval z0, vecval w0) :
		x(x0),
		y(y0),
		z(z0),
		w(w0)
    {
    }

    template <typename vecval>
    constexpr quaternion<vecval>::quaternion(std::initializer_list<vecval> values) : v()
    {
		assert(values.size() == 4);

		this->x = values.begin()[0];
		this->y = values.begin()[1];
		this->z = values.begin()[2];
		this->w = values.begin()[3];
    }
    
    template <typename vecval>
	constexpr quaternion<vecval>::quaternion(vecval value) :
		x(traits<vecval>::zero),
		y(traits<vecval>::zero),
		z(traits<vecval>::zero),
		w(value)
	{
	}

	template <typename vecval>
	constexpr quaternion<vecval>::quaternion(const vecval *values) :
		x(values[0]),
		y(values[1]),
		z(values[2]),
		w(values[3])
	{
	}

	template <typename vecval>
	constexpr quaternion<vecval>::quaternion(const vector<3, vecval> &vec, vecval w0) :
		x(vec.x),
		y(vec.y),
		z(vec.z),
		w(w0)
	{
	}

	template <typename vecval>
	constexpr quaternion<vecval>::quaternion(const vector<4, vecval> &vec) :
		x(vec.x),
		y(vec.y),
		z(vec.z),
		w(vec.w)
	{
	}

	template <typename vecval>
	constexpr quaternion<vecval>::quaternion(const matrix<3, 3, vecval> &mat) : v()
	{
		const auto kx = mat[0][0] - mat[1][1] - mat[2][2];
		const auto ky = mat[1][1] - mat[0][0] - mat[2][2];
		const auto kz = mat[2][2] - mat[0][0] - mat[1][1];
		const auto kw = mat[0][0] + mat[1][1] + mat[2][2];

		size_t n = 0;
		auto k = kw;

		if (kx > k)
		{
			k = kx;
			n = 1;
		}

		if (ky > k)
		{
			k = ky;
			n = 2;
		}

		if (kz > k)
		{
			k = kz;
			n = 3;
		}

		const auto l = vecval(traits<vecval>::type(sqrtl(static_cast<realmax_t>(k) + 1.0) / 2.0));
		const auto f = l * traits<vecval>::type(4);

		if (n == 0)
		{
			this->w = l;
			this->x = (mat[1][2] - mat[2][1]) / f;
			this->y = (mat[2][0] - mat[0][2]) / f;
			this->z = (mat[0][1] - mat[1][0]) / f;
		}
		else if (n == 1)
		{
			this->w = (mat[1][2] - mat[2][1]) / f;
			this->x = l;
			this->y = (mat[0][1] + mat[1][0]) / f;
			this->z = (mat[2][0] + mat[0][2]) / f;
		}
		else if (n == 2)
		{
			this->w = (mat[2][0] - mat[0][2]) / f;
			this->x = (mat[0][1] + mat[1][0]) / f;
			this->y = l;
			this->z = (mat[1][2] + mat[2][1]) / f;
		}
		else if (n == 3)
		{
			this->w = (mat[0][1] - mat[1][0]) / f;
			this->x = (mat[2][0] + mat[0][2]) / f;
			this->y = (mat[1][2] + mat[2][1]) / f;
			this->z = l;
		}
	}

	template <typename vecval>
	constexpr quaternion<vecval>::quaternion(const matrix<4, 4, vecval> &mat)
	{
		const auto kx = mat[0][0] - mat[1][1] - mat[2][2];
		const auto ky = mat[1][1] - mat[0][0] - mat[2][2];
		const auto kz = mat[2][2] - mat[0][0] - mat[1][1];
		const auto kw = mat[0][0] + mat[1][1] + mat[2][2];

		size_t n = 0;
		auto k = kw;

		if (kx > k)
		{
			k = kx;
			n = 1;
		}

		if (ky > k)
		{
			k = ky;
			n = 2;
		}

		if (kz > k)
		{
			k = kz;
			n = 3;
		}

		const auto l = vecval(traits<vecval>::type(sqrtl(static_cast<realmax_t>(k) + 1.0) / 2.0));
		const auto f = l * traits<vecval>::type(4);

		if (n == 0)
		{
			this->w = l;
			this->x = (mat[1][2] - mat[2][1]) / f;
			this->y = (mat[2][0] - mat[0][2]) / f;
			this->z = (mat[0][1] - mat[1][0]) / f;
		}
		else if (n == 1)
		{
			this->w = (mat[1][2] - mat[2][1]) / f;
			this->x = l;
			this->y = (mat[0][1] + mat[1][0]) / f;
			this->z = (mat[2][0] + mat[0][2]) / f;
		}
		else if (n == 2)
		{
			this->w = (mat[2][0] - mat[0][2]) / f;
			this->x = (mat[0][1] + mat[1][0]) / f;
			this->y = l;
			this->z = (mat[1][2] + mat[2][1]) / f;
		}
		else if (n == 3)
		{
			this->w = (mat[0][1] - mat[1][0]) / f;
			this->x = (mat[2][0] + mat[0][2]) / f;
			this->y = (mat[1][2] + mat[2][1]) / f;
			this->z = l;
		}
	}

	template <typename vecval>
	inline quaternion<vecval> &quaternion<vecval>::operator +=(const quaternion<vecval> &quat)
	{
		this->x += quat.x;
		this->y += quat.y;
		this->z += quat.z;
		this->w += quat.w;

		return *this;
	}

	template <typename vecval>
	inline quaternion<vecval> &quaternion<vecval>::operator -=(const quaternion<vecval> &quat)
	{
		this->x -= quat.x;
		this->y -= quat.y;
		this->z -= quat.z;
		this->w -= quat.w;

		return *this;
	}

	template <typename vecval>
	inline quaternion<vecval> &quaternion<vecval>::operator *=(vecval d)
	{
		this->x *= d;
		this->y *= d;
		this->z *= d;
		this->w *= d;

		return *this;
	}

	template <typename vecval>
	inline quaternion<vecval> &quaternion<vecval>::operator /=(vecval d)
	{
		this->x /= d;
		this->y /= d;
		this->z /= d;
		this->w /= d;

		return *this;
	}

	template <typename vecval>
	inline quaternion<vecval> quaternion<vecval>::operator +() const
	{
		return quaternion<vecval>(*this);
	}

	template <typename vecval>
	inline quaternion<vecval> quaternion<vecval>::operator -() const
	{
		return quaternion<vecval>(-this->x, -this->y, -this->z, -this->w);
	}

	template <typename vecval>
	inline bool quaternion<vecval>::operator ==(const quaternion<vecval> &quat) const
	{
		return equals(*this, quat);
	}

	template <typename vecval>
	inline bool quaternion<vecval>::operator !=(const quaternion<vecval> &quat) const
	{
		return !equals(*this, quat);
	}

	template <typename vecval>
	inline vecval quaternion<vecval>::operator [](size_t index) const
	{
		return this->v[index];
	}

	template <typename vecval>
	inline vecval &quaternion<vecval>::operator [](size_t index)
	{
		return this->v[index];
	}

	template <typename vecval>
	template <typename new_vecval>
	inline auto quaternion<vecval>::to() const -> quaternion<new_vecval>
	{
		return {
			new_vecval(this->x),
			new_vecval(this->y),
			new_vecval(this->z),
			new_vecval(this->w)
		};
	}

	template <typename vecval>
	inline quaternion<vecval> operator +(const quaternion<vecval> &q, const quaternion<vecval> &p)
	{
		return quaternion<vecval>(q.x + p.x, q.y + p.y, q.z + p.z, q.w + p.w);
	}

	template <typename vecval>
	inline quaternion<vecval> operator -(const quaternion<vecval> &q, const quaternion<vecval> &p)
	{
		return quaternion<vecval>(q.x - p.x, q.y - p.y, q.z - p.z, q.w - p.w);
	}

	template <typename vecval>
	inline quaternion<vecval> operator *(const quaternion<vecval> &q, const quaternion<vecval> &p)
	{
        quaternion<vecval> result;
        result.x = q.y * p.z - q.z * p.y + q.w * p.x + q.x * p.w;
        result.y = q.z * p.x - q.x * p.z + q.w * p.y + q.y * p.w;
        result.z = q.x * p.y - q.y * p.x + q.w * p.z + q.z * p.w;
        result.w = q.w * p.w - q.x * p.x - q.y * p.y - q.z * p.z;
        return result;
	}

	template <typename vecval>
	inline quaternion<vecval> operator *(const quaternion<vecval> &q, vecval d)
	{
		return quaternion<vecval>(q.x * d, q.y * d, q.z * d, q.w * d);
	}

	template <typename vecval>
	inline quaternion<vecval> operator /(const quaternion<vecval> &q, vecval d)
	{
		assert(d != 0);
		return quaternion<vecval>(q.x / d, q.y / d, q.z / d, q.w / d);
	}

	template <typename vecval>
	inline vecval square_length(const quaternion<vecval> &q)
	{
		return q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w;
	}

	template <typename vecval>
	inline vecval length(const quaternion<vecval> &q)
	{
		return vecval(traits<vecval>::type(sqrtl(square_length(q))));
	}

	template <>
	inline realmax_t length<realmax_t>(const quaternion<realmax_t> &q)
	{
		return sqrtl(square_length(q));
	}

	template <>
	inline int64_t length<int64_t>(const quaternion<int64_t> &q)
	{
		return static_cast<int64_t>(sqrtl(static_cast<realmax_t>(square_length(q))));
	}

	template <typename vecval>
	inline quaternion<vecval> normal(const quaternion<vecval> &q)
	{
		return q / length(q);
	}

	template <typename vecval>
	inline quaternion<vecval> conjugation(const quaternion<vecval> &q)
	{
		return quaternion<vecval>(-q.x, -q.y, -q.z, q.w);
	}

	template <typename vecval>
	inline quaternion<vecval> inverse(const quaternion<vecval> &q)
	{
		return conjugation(q) / length(q);
	}

	template <typename vecval>
	inline quaternion<vecval> &normalize(quaternion<vecval> &q)
	{
		return q /= length(q);
	}

	template <typename vecval>
	inline quaternion<vecval> &conjugate(quaternion<vecval> &q)
	{
		q.x = -q.x;
		q.y = -q.y;
		q.z = -q.z;
		return q;
	}

	template <typename vecval>
	inline quaternion<vecval> &invert(quaternion<vecval> &q)
	{
		return conjugate(q) /= length(q);
	}

	template <typename vecval>
	inline vecval inner(const quaternion<vecval> &q, const quaternion<vecval> &p)
	{
		return q.x * p.x + q.y * p.y + q.z * p.z + q.w * p.w;
	}

	template <typename vecval>
	inline quaternion<vecval> slerp(const quaternion<vecval> &q, const quaternion<vecval> &p, realmax_t t)
	{
		bool is_reverse = false;

		auto cosine = static_cast<realmax_t>(math::inner(q, p));
		realmax_t a, b;

		if (cosine < 0.0)
		{
			cosine = -cosine;
			is_reverse = true;
		}

		auto omega = acosl(cosine);
		auto theta = omega * t;

		auto sine_theta = sinl(theta);
		auto sine_omega = sinl(omega);

		a = cosl(theta) - cosine * sine_theta;
		b = is_reverse ? -sine_theta : sine_theta;

		if (abs(sine_omega) > std::numeric_limits<realmax_t>::epsilon())
		{
			a /= sine_omega;
			b /= sine_omega;
		}

		return math::normal(
			q * traits<vecval>::type(a) +
			p * traits<vecval>::type(b));
	}
}

#pragma once

namespace laplace::math
{
	template <typename vecval>
	constexpr complex<vecval>::complex(vecval x0, vecval y0) :
		x(x0),
		y(y0)
	{
	}

	template <typename vecval>
	constexpr complex<vecval>::complex(std::initializer_list<vecval> values) :
		x(values[0]),
		y(values[1])
	{
	}

	template <typename vecval>
	constexpr complex<vecval>::complex(vecval real) :
		x(real),
		y(traits<vecval>::zero)
	{
	}

	template <typename vecval>
	constexpr complex<vecval>::complex(const vecval *values) :
		x(v[0]),
		y(v[1])
	{
	}

	template <typename vecval>
	constexpr complex<vecval>::complex(const vector<2, vecval> &vec) :
		x(vec.x),
		y(vec.y)
	{
	}

	template <typename vecval>
	constexpr complex<vecval>::complex(const matrix<2, 2, vecval> &mat) :
		x((mat.m[0][0] + mat.m[1][1]) / vecval(traits<vecval>::type(2))),
		y((mat.m[0][1] - mat.m[1][0]) / vecval(traits<vecval>::type(2)))
	{
	}

	template <typename vecval>
	inline complex<vecval> &complex<vecval>::operator +=(const complex<vecval> &complex)
	{
		this->real += complex.real;
		this->imag += complex.imag;

		return *this;
	}

	template <typename vecval>
	inline complex<vecval> &complex<vecval>::operator -=(const complex<vecval> &complex)
	{
		this->real -= complex.real;
		this->imag -= complex.imag;

		return *this;
	}

	template <typename vecval>
	inline complex<vecval> &complex<vecval>::operator *=(const complex<vecval> &complex)
	{
		return *this = *this * complex;
	}

	template <typename vecval>
	inline complex<vecval> &complex<vecval>::operator /=(const complex<vecval> &complex)
	{
		return *this = *this / complex;
	}

	template <typename vecval>
	inline complex<vecval> &complex<vecval>::operator +=(vecval d)
	{
		this->real += d;
		return *this;
	}

	template <typename vecval>
	inline complex<vecval> &complex<vecval>::operator -=(vecval d)
	{
		this->real -= d;
		return *this;
	}

	template <typename vecval>
	inline complex<vecval> &complex<vecval>::operator *=(vecval d)
	{
		this->real *= d;
		this->imag *= d;
		return *this;
	}

	template <typename vecval>
	inline complex<vecval> &complex<vecval>::operator /=(vecval d)
	{
		this->real /= d;
		this->imag /= d;
		return *this;
	}

	template <typename vecval>
	inline complex<vecval> complex<vecval>::operator +() const
	{
		return *this;
	}

	template <typename vecval>
	inline complex<vecval> complex<vecval>::operator -() const
	{
		return complex<vecval>(-this->real, -this->imag);
	}

	template <typename vecval>
	inline vecval complex<vecval>::operator [](size_t index) const
	{
		return this->v[index];
	}

	template <typename vecval>
	inline vecval &complex<vecval>::operator [](size_t index)
	{
		return this->v[index];
	}

	template <typename vecval>
	inline vecval re(const complex<vecval> &a)
	{
		return a.real;
	}

	template <typename vecval>
	inline vecval im(const complex<vecval> &a)
	{
		return a.imag;
	}

	template <typename vecval>
	inline complex<vecval> operator +(const complex<vecval> &a, const complex<vecval> &b)
	{
		return complex<vecval>(a.real + b.real, a.imag + b.imag);
	}

	template <typename vecval>
	inline complex<vecval> operator -(const complex<vecval> &a, const complex<vecval> &b)
	{
		return complex<vecval>(a.real - b.real, a.imag - b.imag);
	}

	template <typename vecval>
	inline complex<vecval> operator *(const complex<vecval> &a, const complex<vecval> &b)
	{
		return complex<vecval>(a.real * b.real - a.imag * b.imag, a.real * b.imag + a.imag * b.real);
	}

	template <typename vecval>
	inline complex<vecval> operator /(const complex<vecval> &a, const complex<vecval> &b)
	{
		complex<vecval> result;
		vecval k = vecval(1. / square_length(b));
		result.real = (a.real * b.real + a.imag * b.imag) * k;
		result.imag = (a.imag * b.real - a.real * b.imag) * k;
		return result;
	}

	template <typename vecval>
	inline complex<vecval> operator +(const complex<vecval> &a, vecval b)
	{
		return complex<vecval>(a.real + b, a.imag);
	}

	template <typename vecval>
	inline complex<vecval> operator -(const complex<vecval> &a, vecval b)
	{
		return complex<vecval>(a.real - b, a.imag);
	}

	template <typename vecval>
	inline complex<vecval> operator *(const complex<vecval> &a, vecval b)
	{
		return complex<vecval>(a.real * b, a.imag * b);
	}

	template <typename vecval>
	inline complex<vecval> operator /(const complex<vecval> &a, vecval b)
	{
		return complex<vecval>(a.real / b, a.imag / b);
	}

	template <typename vecval>
	inline complex<vecval> operator +(vecval a, const complex<vecval> &b)
	{
		return complex<vecval>(a + b.real, b.imag);
	}

	template <typename vecval>
	inline complex<vecval> operator -(vecval a, const complex<vecval> &b)
	{
		return complex<vecval>(a - b.real, -b.imag);
	}

	template <typename vecval>
	inline complex<vecval> operator *(vecval a, const complex<vecval> &b)
	{
		return complex<vecval>(a * b.real, a * b.imag);
	}

	template <typename vecval>
	inline complex<vecval> operator /(vecval a, const complex<vecval> &b)
	{
		vecval k = vecval(1. / square_length(b));
		return complex<vecval>(a * b.real * k, -a * b.imag * k);
	}

	template <typename vecval>
	inline auto operator ==(const complex<vecval> &a, const complex<vecval> &b) -> bool
	{
		return equals(a, b);
	}

	template <typename vecval>
	inline auto operator !=(const complex<vecval> &a, const complex<vecval> &b) -> bool
	{
		return equals(a, b);
	}

	template <typename vecval>
	inline vecval arg(const complex<vecval> &a)
	{
		return vecval(atan2(a.imag, a.real));
	}

	template <typename vecval>
	inline vecval square_length(const complex<vecval> &a)
	{
		return a.real * a.real + a.imag + a.imag;
	}

	template <typename vecval>
	inline vecval length(const complex<vecval> &a)
	{
		return vecval(sqrt(square_length(a)));
	}

	template <>
	inline realmax_t length<realmax_t>(const complex<realmax_t> &a)
	{
		return sqrtl(square_length(a));
	}

	template <>
	inline int64_t length<int64_t>(const complex<int64_t> &a)
	{
		return static_cast<int64_t>(sqrtl(static_cast<realmax_t>(square_length(a))));
	}

	template <typename vecval>
	inline complex<vecval> normal(const complex<vecval> &a)
	{
		return a / length(a);
	}

	template <typename vecval>
	inline complex<vecval> conjugation(const complex<vecval> &a)
	{
		return complex<vecval>(a.real, -a.imag);
	}

	template <typename vecval>
	inline complex<vecval> inverse(const complex<vecval> &a)
	{
		vecval d = square_length(a);
		return complex<vecval>(a.real / d, -a.imag / d);
	}

	template <typename vecval>
	inline complex<vecval> &normalize(complex<vecval> &a)
	{
		vecval k = vecval(1. / length(a));
		a.real *= k;
		a.imag *= k;
		return a;
	}

	template <typename vecval>
	inline complex<vecval> &conjugate(complex<vecval> &a)
	{
		a.imag = -a.imag;
		return a;
	}

	template <typename vecval>
	inline complex<vecval> &invert(complex<vecval> &a)
	{
		vecval k = vecval(1. / square_length(a));
		a.real *= k;
		a.imag *= -k;
		return a;
	}
}

#pragma once

#include "traits.h"
#include <initializer_list>
#include <cstdint>

namespace laplace::math
{
	/*	Complex number.
	 *
	 *	C = x + y i, where i^2 = -1
	 */
	template <typename vecval>
	class complex
	{
	public:
		typedef vecval type;
		
		static constexpr size_t size = 2;

		union {
			struct { vecval x, y; };
			struct { vecval real, imag; };
			struct { vecval v[2]; };
		};

		constexpr complex() = default;
		constexpr complex(vecval x, vecval y);
		constexpr complex(std::initializer_list<vecval> values);

        explicit constexpr complex(vecval real);
        explicit constexpr complex(const vecval *v);
		explicit constexpr complex(const vector<2, vecval> &vec);
		explicit constexpr complex(const matrix<2, 2, vecval> &mat);

		complex<vecval> &operator +=(const complex<vecval> &co);
		complex<vecval> &operator -=(const complex<vecval> &co);
		complex<vecval> &operator *=(const complex<vecval> &co);
		complex<vecval> &operator /=(const complex<vecval> &co);

		complex<vecval> &operator +=(vecval d);
		complex<vecval> &operator -=(vecval d);
		complex<vecval> &operator *=(vecval d);
		complex<vecval> &operator /=(vecval d);

		complex<vecval> operator +() const;
		complex<vecval> operator -() const;

		vecval operator [](size_t index) const;
		vecval &operator [](size_t index);
	};

	template <typename vecval>
	vecval re(const complex<vecval> &a);

	template <typename vecval>
	vecval im(const complex<vecval> &a);

	template <typename vecval>
	complex<vecval> operator +(const complex<vecval> &a, const complex<vecval> &b);

	template <typename vecval>
	complex<vecval> operator -(const complex<vecval> &a, const complex<vecval> &b);

	template <typename vecval>
	complex<vecval> operator *(const complex<vecval> &a, const complex<vecval> &b);

	template <typename vecval>
	complex<vecval> operator /(const complex<vecval> &a, const complex<vecval> &b);

	template <typename vecval>
	complex<vecval> operator +(const complex<vecval> &a, vecval b);

	template <typename vecval>
	complex<vecval> operator -(const complex<vecval> &a, vecval b);

	template <typename vecval>
	complex<vecval> operator *(const complex<vecval> &a, vecval b);

	template <typename vecval>
	complex<vecval> operator /(const complex<vecval> &a, vecval b);

	template <typename vecval>
	complex<vecval> operator +(vecval a, const complex<vecval> &b);

	template <typename vecval>
	complex<vecval> operator -(vecval a, const complex<vecval> &b);

	template <typename vecval>
	complex<vecval> operator *(vecval a, const complex<vecval> &b);

	template <typename vecval>
	complex<vecval> operator /(vecval a, const complex<vecval> &b);

	template <typename vecval>
	auto operator ==(const complex<vecval> &a, const complex<vecval> &b) -> bool;

	template <typename vecval>
	auto operator !=(const complex<vecval> &a, const complex<vecval> &b) -> bool;

	template <typename vecval>
	vecval arg(const complex<vecval> &a);

	template <typename vecval>
	vecval square_length(const complex<vecval> &a);

	template <typename vecval>
	vecval length(const complex<vecval> &a);

	template <typename vecval>
	complex<vecval> normal(const complex<vecval> &a);

	template <typename vecval>
	complex<vecval> conjugation(const complex<vecval> &a);

	template <typename vecval>
	complex<vecval> inverse(const complex<vecval> &a);

	template <typename vecval>
	complex<vecval> &normalize(complex<vecval> &a);

	template <typename vecval>
	complex<vecval> &conjugate(complex<vecval> &a);

	template <typename vecval>
	complex<vecval> &invert(complex<vecval> &a);
}

#include "complex.impl.h"

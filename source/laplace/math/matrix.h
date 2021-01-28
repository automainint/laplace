#pragma once

#include "traits.h"
#include <limits>
#include <initializer_list>
#include <cmath>

namespace laplace::math
{
	template <size_t rowcount, size_t colcount, typename vecval>
	class matrix
	{
	public:
		using type = vecval;

        using ref = matrix<rowcount, colcount, vecval> &;
        using cref = const matrix<rowcount, colcount, vecval> &;
        
        static constexpr size_t size            = rowcount * colcount;
        static constexpr size_t row_count       = rowcount;
        static constexpr size_t column_count    = colcount;

		union {
			struct { vecval m[rowcount][colcount]; };
			struct { vecval v[size]; };
		};

        constexpr matrix() = default;
        constexpr matrix(std::initializer_list<vecval> values);

        explicit constexpr matrix(vecval d);
        explicit constexpr matrix(const vecval *v);

        auto operator +=(cref mat) -> ref;
        auto operator -=(cref mat) -> ref;
        auto operator *=(vecval d) -> ref;
        auto operator /=(vecval d) -> ref;

        auto operator +() const -> matrix<rowcount, colcount, vecval>;
        auto operator -() const -> matrix<rowcount, colcount, vecval>;

        auto operator [](size_t index) -> vecval *;
        auto operator [](size_t index) const -> const vecval *;

        void set_row(size_t i, const vector<colcount, vecval> &row);
        void set_column(size_t j, const vector<rowcount, vecval> &column);

        auto get_row(size_t i) const -> vector<colcount, vecval>;
        auto get_column(size_t j) const -> vector<rowcount, vecval>;
	};

	template <typename vecval>
	class matrix<2, 2, vecval>
	{
	public:
		using type = vecval;

        using ref = matrix<2, 2, vecval> &;
        using cref = const matrix<2, 2, vecval> &;

        static constexpr size_t size = 4;
        static constexpr size_t row_count = 2;
        static constexpr size_t column_count = 2;

		union {
			struct { vecval m[2][2]; };
			struct { vecval v[4]; };
		};

        constexpr matrix() = default;
        constexpr matrix(std::initializer_list<vecval> values);

		explicit constexpr matrix(vecval d);
		explicit constexpr matrix(const vecval *v);
		explicit constexpr matrix(const matrix<3, 3, vecval> &mat);
		explicit constexpr matrix(const matrix<4, 4, vecval> &mat);
		explicit constexpr matrix(const complex<vecval> &complex);

        constexpr matrix(
            const vector<2, vecval> &row0,
            const vector<2, vecval> &row1
        );

		auto operator +=(cref mat) -> ref;
		auto operator -=(cref mat) -> ref;
		auto operator *=(vecval d) -> ref;
		auto operator /=(vecval d) -> ref;

		auto operator +() const -> matrix<2, 2, vecval>;
		auto operator -() const -> matrix<2, 2, vecval>;

		auto operator [](size_t index) -> vecval *;
		auto operator [](size_t index) const -> const vecval *;

        void set_row(size_t i, const vector<2, vecval> &row);
        void set_column(size_t j, const vector<2, vecval> &column);

        auto get_row(size_t i) const -> vector<2, vecval>;
		auto get_column(size_t j) const -> vector<2, vecval>;
	};

	template <typename vecval>
	class matrix<3, 3, vecval>
	{
	public:
		using type = vecval;

        using ref = matrix<3, 3, vecval> &;
        using cref = matrix<3, 3, vecval> &;

        static constexpr size_t size = 9;
        static constexpr size_t row_count = 3;
        static constexpr size_t column_count = 3;

		union {
			struct { vecval m[3][3]; };
			struct { vecval v[9]; };
		};

        constexpr matrix() = default;
        constexpr matrix(std::initializer_list<vecval> values);

		explicit constexpr matrix(vecval d);
		explicit constexpr matrix(const vecval *v);
		explicit constexpr matrix(const matrix<2, 2, vecval> &mat);
		explicit constexpr matrix(const matrix<4, 4, vecval> &mat);
		explicit constexpr matrix(const quaternion<vecval> &quat);

        constexpr matrix(
            const vector<3, vecval> &row0,
            const vector<3, vecval> &row1,
            const vector<3, vecval> &row2
        );

		auto operator +=(cref mat) -> ref;
		auto operator -=(cref mat) -> ref;
		auto operator *=(vecval d) -> ref;
		auto operator /=(vecval d) -> ref;

		auto operator +() const -> matrix<3, 3, vecval>;
		auto operator -() const -> matrix<3, 3, vecval>;

		auto operator [](size_t index) -> vecval *;
		auto operator [](size_t index) const -> const vecval *;

        void set_row(size_t i, const vector<3, vecval> &row);
        void set_column(size_t j, const vector<3, vecval> &column);

        auto get_row(size_t i) const -> vector<3, vecval>;
		auto get_column(size_t j) const -> vector<3, vecval>;
	};

	template <typename vecval>
	class matrix<4, 4, vecval>
	{
	public:
		using type = vecval;

        using ref = matrix<4, 4, vecval> &;
        using cref = const matrix<4, 4, vecval> &;

        static constexpr size_t size = 16;
        static constexpr size_t row_count = 4;
        static constexpr size_t column_count = 4;

		union {
			struct { vecval m[4][4]; };
			struct { vecval v[16]; };
		};

        constexpr matrix() = default;
        constexpr matrix(std::initializer_list<vecval> values);

		explicit constexpr matrix(vecval d);
		explicit constexpr matrix(const vecval *v);
		explicit constexpr matrix(const matrix<2, 2, vecval> &mat);
		explicit constexpr matrix(const matrix<3, 3, vecval> &mat);
		explicit constexpr matrix(const quaternion<vecval> &quat);

        constexpr matrix(
            const vector<4, vecval> &row0,
            const vector<4, vecval> &row1,
            const vector<4, vecval> &row2,
            const vector<4, vecval> &row3
        );

		auto operator +=(cref mat) -> ref;
		auto operator -=(cref mat) -> ref;
		auto operator *=(vecval d) -> ref;
		auto operator /=(vecval d) -> ref;

		auto operator +() const -> matrix<4, 4, vecval>;
		auto operator -() const -> matrix<4, 4, vecval>;

		auto operator [](size_t index) -> vecval *;
		auto operator [](size_t index) const -> const vecval *;

        void set_row(size_t i, const vector<4, vecval> &row);
        void set_column(size_t j, const vector<4, vecval> &column);

        auto get_row(size_t i) const -> vector<4, vecval>;
		auto get_column(size_t j) const -> vector<4, vecval>;
	};

	template <size_t rowcount, size_t colcount, typename vecval>
	auto operator +(const matrix<rowcount, colcount, vecval> &a, const matrix<rowcount, colcount, vecval> &b) -> matrix<rowcount, colcount, vecval>;

	template <size_t rowcount, size_t colcount, typename vecval>
	auto operator -(const matrix<rowcount, colcount, vecval> &a, const matrix<rowcount, colcount, vecval> &b) -> matrix<rowcount, colcount, vecval>;

	template <size_t rowcount, size_t colcount, typename vecval>
	auto operator *(const matrix<rowcount, colcount, vecval> &mat, vecval d) -> matrix<rowcount, colcount, vecval>;

	template <size_t rowcount, size_t colcount, typename vecval>
	auto operator /(const matrix<rowcount, colcount, vecval> &mat, vecval d) -> matrix<rowcount, colcount, vecval>;

	template <size_t rowcount, size_t colcount, size_t K, typename vecval>
	auto operator *(const matrix<rowcount, colcount, vecval> &a, const matrix<colcount, K, vecval> &b) -> matrix<rowcount, K, vecval>;

	template <size_t rowcount, size_t colcount, typename vecval>
	auto operator *(const matrix<rowcount, colcount, vecval> &mat, const vector<colcount, vecval> &vec) -> vector<rowcount, vecval>;

	template <typename vecval>
	auto operator *(const matrix<3, 3, vecval> &mat, const vector<2, vecval> &vec) -> vector<2, vecval>;

	template <typename vecval>
	auto operator *(const matrix<4, 4, vecval> &mat, const vector<3, vecval> &vec) -> vector<3, vecval>;

	template <size_t rowcount, size_t colcount, typename vecval>
	auto operator *(const vector<rowcount, vecval> &vec, const matrix<rowcount, colcount, vecval> &mat) -> vector<colcount, vecval>;

	template <typename vecval>
	auto operator *(const vector<2, vecval> &vec, const matrix<3, 3, vecval> &mat) -> vector<2, vecval>;

	template <typename vecval>
	auto operator *(const vector<3, vecval> &vec, const matrix<4, 4, vecval> &mat) -> vector<3, vecval>;

    template <size_t rowcount, size_t colcount, typename vecval>
    auto operator ==(const matrix<rowcount, colcount, vecval> &a, const matrix<rowcount, colcount, vecval> &b) -> bool;

    template <size_t rowcount, size_t colcount, typename vecval>
    auto operator !=(const matrix<rowcount, colcount, vecval> &a, const matrix<rowcount, colcount, vecval> &b) -> bool;

    template <size_t rowcount, size_t colcount, typename vecval>
    auto add(const matrix<rowcount, colcount, vecval> &a, const matrix<rowcount, colcount, vecval> &b) -> matrix<rowcount, colcount, vecval>;

    template <size_t rowcount, size_t colcount, typename vecval>
    auto subtract(const matrix<rowcount, colcount, vecval> &a, const matrix<rowcount, colcount, vecval> &b) ->matrix<rowcount, colcount, vecval>;

    template <size_t colcount, typename vecval>
	auto trace(const matrix<colcount, colcount, vecval> &mat) -> vecval;

	template <size_t colcount, typename vecval>
	auto det(const matrix<colcount, colcount, vecval> &mat) -> vecval;

	template <size_t colcount, typename vecval>
	auto inverse(const matrix<colcount, colcount, vecval> &mat) -> matrix<colcount, colcount, vecval>;

	template <size_t rowcount, size_t colcount, typename vecval>
	auto transpose(const matrix<rowcount, colcount, vecval> &mat) -> matrix<rowcount, colcount, vecval>;

    template <typename vecval>
    auto normalize_rotation(const matrix<3, 3, vecval> &mat) -> matrix<3, 3, vecval>;
}

#include "matrix.impl.h"

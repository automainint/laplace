#ifndef __laplace__math_vector__
#define __laplace__math_vector__

#include "traits.h"
#include <cassert>
#include <initializer_list>

namespace laplace::math {
  template <size_t valcount, typename vecval>
  class vector {
  public:
    typedef vecval type;

    static constexpr size_t size = valcount;

    vecval v[valcount];

    constexpr vector() = default;
    constexpr vector(std::initializer_list<vecval> values);

    explicit constexpr vector(vecval v);
    explicit constexpr vector(const vecval *v);

    vector<valcount, vecval> &operator+=(
        const vector<valcount, vecval> &v);
    vector<valcount, vecval> &operator-=(
        const vector<valcount, vecval> &v);
    vector<valcount, vecval> &operator*=(vecval d);
    vector<valcount, vecval> &operator/=(vecval d);
    vector<valcount, vecval> &operator*=(
        const vector<valcount, vecval> &v);
    vector<valcount, vecval> &operator/=(
        const vector<valcount, vecval> &v);

    vector<valcount, vecval> operator+() const;
    vector<valcount, vecval> operator-() const;

    vecval  operator[](size_t index) const;
    vecval &operator[](size_t index);
  };

  template <typename vecval>
  class vector<2, vecval> {
  public:
    typedef vecval type;

    static constexpr size_t size = 2;

    union {
      struct {
        vecval x, y;
      };
      struct {
        vecval v[2];
      };
    };

    constexpr vector() = default;
    constexpr vector(vecval x, vecval y);
    constexpr vector(std::initializer_list<vecval> values);

    explicit constexpr vector(vecval v);
    explicit constexpr vector(const vecval *v);
    explicit constexpr vector(const complex<vecval> &complex);
    explicit constexpr vector(const vector<3, vecval> &v);
    explicit constexpr vector(const vector<4, vecval> &v);

    vector<2, vecval> &operator+=(const vector<2, vecval> &v);
    vector<2, vecval> &operator-=(const vector<2, vecval> &v);
    vector<2, vecval> &operator*=(vecval d);
    vector<2, vecval> &operator/=(vecval d);
    vector<2, vecval> &operator*=(const vector<2, vecval> &v);
    vector<2, vecval> &operator/=(const vector<2, vecval> &v);

    vector<2, vecval> operator+() const;
    vector<2, vecval> operator-() const;

    vecval  operator[](size_t index) const;
    vecval &operator[](size_t index);
  };

  template <typename vecval>
  class vector<3, vecval> {
  public:
    typedef vecval type;

    static constexpr size_t size = 3;

    union {
      struct {
        vecval x, y, z;
      };
      struct {
        vecval red, green, blue;
      };
      struct {
        vecval v[3];
      };
    };

    constexpr vector() = default;
    constexpr vector(vecval x, vecval y, vecval z);
    constexpr vector(std::initializer_list<vecval> values);

    explicit constexpr vector(vecval v);
    explicit constexpr vector(const vecval *v);
    explicit constexpr vector(const vector<2, vecval> &v,
                              vecval                   z);
    explicit constexpr vector(const vector<4, vecval> &v);

    vector<3, vecval> &operator+=(const vector<3, vecval> &v);
    vector<3, vecval> &operator-=(const vector<3, vecval> &v);
    vector<3, vecval> &operator*=(vecval d);
    vector<3, vecval> &operator/=(vecval d);
    vector<3, vecval> &operator*=(const vector<3, vecval> &v);
    vector<3, vecval> &operator/=(const vector<3, vecval> &v);

    vector<3, vecval> operator+() const;
    vector<3, vecval> operator-() const;

    vecval  operator[](size_t index) const;
    vecval &operator[](size_t index);
  };

  template <typename vecval>
  class vector<4, vecval> {
  public:
    typedef vecval type;

    static constexpr size_t size = 4;

    union {
      struct {
        vecval x, y, z, w;
      };
      struct {
        vecval red, green, blue, alpha;
      };
      struct {
        vecval v[4];
      };
    };

    constexpr vector() = default;
    constexpr vector(vecval x, vecval y, vecval z, vecval w);
    constexpr vector(std::initializer_list<vecval> values);

    explicit constexpr vector(vecval v);
    explicit constexpr vector(const vecval *v);
    explicit constexpr vector(const vector<2, vecval> &v,
                              vecval z, vecval w);
    explicit constexpr vector(const vector<3, vecval> &v,
                              vecval                   w);
    explicit constexpr vector(const quaternion<vecval> &quat);

    vector<4, vecval> &operator+=(const vector<4, vecval> &v);
    vector<4, vecval> &operator-=(const vector<4, vecval> &v);
    vector<4, vecval> &operator*=(vecval d);
    vector<4, vecval> &operator/=(vecval d);
    vector<4, vecval> &operator*=(const vector<4, vecval> &v);
    vector<4, vecval> &operator/=(const vector<4, vecval> &v);

    vector<4, vecval> operator+() const;
    vector<4, vecval> operator-() const;

    vecval  operator[](size_t index) const;
    vecval &operator[](size_t index);
  };

  template <size_t valcount, typename vecval>
  vector<valcount, vecval> operator+(
      const vector<valcount, vecval> &u,
      const vector<valcount, vecval> &v);

  template <size_t valcount, typename vecval>
  vector<valcount, vecval> operator-(
      const vector<valcount, vecval> &u,
      const vector<valcount, vecval> &v);

  template <size_t valcount, typename vecval>
  vector<valcount, vecval> operator*(
      const vector<valcount, vecval> &v, vecval d);

  template <size_t valcount, typename vecval>
  vector<valcount, vecval> operator/(
      const vector<valcount, vecval> &v, vecval d);

  template <size_t valcount, typename vecval>
  vector<valcount, vecval> operator*(
      const vector<valcount, vecval> &u,
      const vector<valcount, vecval> &v);

  template <size_t valcount, typename vecval>
  vector<valcount, vecval> operator/(
      const vector<valcount, vecval> &u,
      const vector<valcount, vecval> &v);

  template <size_t valcount, typename vecval>
  vector<valcount, vecval> operator*(
      vecval d, const vector<valcount, vecval> &v);

  template <size_t valcount, typename vecval>
  auto operator==(const vector<valcount, vecval> &a,
                  const vector<valcount, vecval> &b) -> bool;

  template <size_t valcount, typename vecval>
  auto operator!=(const vector<valcount, vecval> &a,
                  const vector<valcount, vecval> &b) -> bool;

  template <size_t valcount, typename vecval>
  vecval square_length(const vector<valcount, vecval> &v);

  template <size_t valcount, typename vecval>
  vecval length(const vector<valcount, vecval> &v);

  template <size_t valcount, typename vecval>
  vecval dot(const vector<valcount, vecval> &u,
             const vector<valcount, vecval> &v);

  template <size_t valcount, typename vecval>
  vector<valcount, vecval> normal(const vector<valcount, vecval> &v);

  template <size_t valcount, typename vecval>
  vector<valcount, vecval> &normalize(vector<valcount, vecval> &v);

  template <typename vecval>
  vector<3, vecval> cross(const vector<3, vecval> &u,
                          const vector<3, vecval> &v);

  template <size_t valcount, typename vecval>
  vector<valcount, vecval> project(
      const vector<valcount, vecval> &vec,
      const vector<valcount, vecval>  norm);

  template <typename vecval>
  vector<2, vecval> rotate(const vector<2, vecval> &v,
                           const complex<vecval> &  a);

  template <typename vecval>
  vector<3, vecval> rotate(const vector<3, vecval> & v,
                           const quaternion<vecval> &q);

  template <typename vecval>
  vector<4, vecval> rotate(const vector<4, vecval> & v,
                           const quaternion<vecval> &q);
}

#include "vector.impl.h"

#endif

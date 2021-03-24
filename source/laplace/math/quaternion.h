/*  laplace/math/quaternion.h
 *
 *  Copyright (c) 2021 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef laplace_math_quaternion_h
#define laplace_math_quaternion_h

#include "vector.h"

namespace laplace::math {
  /*	Quaternion.
   *
   *	Q = x i + y j + z k + w,
   *	where i^2 = j^2 = k^2 = -1.
   */
  template <typename type_>
  class quaternion : public vector<4, type_> {
  public:
    using vector<4, type_>::vector;

    constexpr explicit quaternion(const type_ x) noexcept {
      this->v[this->n_w] = x;
    }
  };
}

#include "quaternion.impl.h"

#endif

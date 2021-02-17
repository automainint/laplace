/*  laplace/math/complex.h
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

#ifndef __laplace__math_complex__
#define __laplace__math_complex__

#include "vector.h"

namespace laplace::math {
  /*	Complex number.
   *
   *	C = x + y i, where i^2 = -1
   */
  template <typename type_>
  class complex : public vector<2, type_> {
  public:
    using vector<2, type_>::vector;
  };
}

#include "complex.impl.h"

#endif

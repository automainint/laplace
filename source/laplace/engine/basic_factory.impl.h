/*  Copyright (c) 2021 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef laplace_engine_basic_factory_impl_h
#define laplace_engine_basic_factory_impl_h

namespace laplace::engine {
  template <typename impact_type_>
  inline auto basic_factory::make(span_cbyte seq)
      -> ptr_prime_impact {
    return std::make_unique<impact_type_>(impact_type_::decode(seq));
  }
}

#endif

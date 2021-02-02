/*  laplace/engine/basic_factory.impl.h
 *
 *      Basic factory inlines.
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

#ifndef __laplace__engine_basic_factory_impl__
#define __laplace__engine_basic_factory_impl__

namespace laplace::engine {
  template <typename impact_type>
  inline auto basic_factory::make(cref_vbyte seq) -> ptr_impact {
    return std::make_shared<impact_type>(
        std::move(impact_type::decode(seq)));
  }
}

#endif

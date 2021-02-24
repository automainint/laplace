/*  laplace/engine/access/world.impl.h
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

#ifndef laplace_engine_access_world_impl_h
#define laplace_engine_access_world_impl_h

namespace laplace::engine::access {
  template <typename distribution>
  inline auto world::random(distribution &dist) ->
      typename distribution::result_type {
    if (this->m_mode == sync) {
      return dist(get_random_engine());
    }

    return distribution::result_type(0);
  }
}

#endif

/*  laplace/engine/basic_entity.impl.h
 *
 *      Basic class for any gameplay object. Inline methods.
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

#ifndef laplace_engine_basic_entity_impl_h
#define laplace_engine_basic_entity_impl_h

namespace laplace::engine {
  inline auto basic_entity::get_by_id(sl::index id) -> intval {
    return this->get(this->index_of(id));
  }

  inline auto basic_entity::locked_get_by_id(sl::index id) const
      -> intval {
    return this->locked_get(this->index_of(id));
  }
}

#endif

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

#ifndef __laplace__engine_basic_entity_impl__
#define __laplace__engine_basic_entity_impl__

namespace laplace::engine {
  inline auto basic_entity::get_by_id(size_t id) -> int64_t {
    return this->get(this->index_of(id));
  }

  inline auto basic_entity::locked_get_by_id(size_t id) const
      -> int64_t {
    return this->locked_get(this->index_of(id));
  }
}

#endif

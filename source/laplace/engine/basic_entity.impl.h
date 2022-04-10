/*  Copyright (c) 2021 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef LAPLACE_ENGINE_BASIC_ENTITY_IMPL_H
#define LAPLACE_ENGINE_BASIC_ENTITY_IMPL_H

namespace laplace::engine {
  inline auto basic_entity::get_by_id(sl::index id) noexcept
      -> intval {
    return this->get(this->index_of(id));
  }
}

#endif

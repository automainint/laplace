/*  Copyright (c) 2021 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef LAPLACE_ENGINE_BASIC_ENTITY_PREDEF_H
#define LAPLACE_ENGINE_BASIC_ENTITY_PREDEF_H

#include <memory>
#include <vector>

namespace laplace::engine {
  class basic_entity;

  using ref_entity  = basic_entity &;
  using cref_entity = const basic_entity &;

  using ptr_entity  = std::shared_ptr<basic_entity>;
  using vptr_entity = std::vector<ptr_entity>;
}

#endif

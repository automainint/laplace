/*  laplace/engine/basic_entity.predef.h
 *
 *      Basic class for any gameplay object. Minimal
 *      declarations.
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

#ifndef __laplace__engine_basic_entity_predef__
#define __laplace__engine_basic_entity_predef__

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

/*  laplace/engine/basic_impact.predef.h
 *
 *      Impact declarations.
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

#ifndef __laplace__engine_basic_impact_predef__
#define __laplace__engine_basic_impact_predef__

#include <memory>
#include <vector>

namespace laplace::engine {
  class basic_impact;
  using ptr_impact  = std::shared_ptr<basic_impact>;
  using vptr_impact = std::vector<ptr_impact>;
}

#endif

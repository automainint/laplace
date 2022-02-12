/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef LAPLACE_ENGINE_BASIC_IMPACT_PREDEF_H
#define LAPLACE_ENGINE_BASIC_IMPACT_PREDEF_H

#include "../core/defs.h"
#include <memory>
#include <vector>

namespace laplace::engine {
  class basic_impact;
  using ptr_impact  = std::unique_ptr<basic_impact>;
  using vptr_impact = sl::vector<ptr_impact>;
}

#endif

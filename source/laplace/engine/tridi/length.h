/*  Copyright (c) 2021 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef laplace_engine_tridi_length_h
#define laplace_engine_tridi_length_h

#include "../defs.h"

namespace laplace::engine::tridi {
  [[nodiscard]] auto length(vec3i const &v) noexcept -> intval;
}

#endif

/*  laplace/stem/s_native_api.cpp
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

#include "native_api.h"

namespace laplace::stem {
  native_api::native_api() { }
  native_api::~native_api() { }

  auto native_api::perform(size_t id, cref_vbyte args) -> vbyte {
    return {};
  }
}

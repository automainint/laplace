/*  laplace/stem/native_api.h
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

#ifndef __laplace__stem_native_api__
#define __laplace__stem_native_api__

#include "../vm/agent.h"

namespace laplace::stem {
  class native_api final : public vm::agent {
  public:
    native_api();
    ~native_api();

    auto perform(size_t id, cref_vbyte args) -> vbyte final;
  };
}

#endif

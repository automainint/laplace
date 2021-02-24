/*  laplace/vm/environment.h
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

#ifndef laplace_vm_environment_h
#define laplace_vm_environment_h

#include "../core/defs.h"
#include "agent.predef.h"

namespace laplace::vm {
  class environment {
  public:
    environment();
    ~environment();

    auto add(size_t unique_id, ptr_agent a) -> size_t;
    auto get_id(size_t unique_id) -> size_t;
    auto perform(size_t id, cref_vbyte args) -> vbyte;

  private:
    std::vector<std::pair<size_t, ptr_agent>> m_ids;
  };
}

#endif

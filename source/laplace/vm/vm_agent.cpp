/*  laplace/vm/vm_agent.cpp
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

#include "agent.h"

namespace laplace::vm {
  void agent::set_environment(ptr_environment env) {
    m_env = env;
  }

  auto agent::perform(size_t id, span_cbyte args) -> vbyte {
    return {};
  }

  auto agent::get_env() const -> ptr_environment {
    return m_env.lock();
  }
}

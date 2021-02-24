/*  laplace/vm/agent.predef.h
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

#ifndef laplace_vm_agent_predef_h
#define laplace_vm_agent_predef_h

#include <memory>

namespace laplace::vm {
  class agent;
  using ptr_agent = std::shared_ptr<agent>;
}

#endif

/*  laplace/vm/agent.h
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

#ifndef laplace_vm_agent_h
#define laplace_vm_agent_h

#include "../core/defs.h"
#include "environment.predef.h"

namespace laplace::vm {
  class agent : public std::enable_shared_from_this<agent> {
  public:
    static constexpr size_t id_undefined = -1;

    virtual ~agent() = default;

    void set_environment(ptr_environment env);

    virtual auto perform(size_t id, cref_vbyte args) -> vbyte;

    auto get_env() const -> ptr_environment;

  private:
    std::weak_ptr<environment> m_env;
  };
}

#endif

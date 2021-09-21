/*  laplace/engine/action/remove.h
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

#ifndef laplace_engine_action_remove_h
#define laplace_engine_action_remove_h

#include "../basic_impact.h"

namespace laplace::engine::action {
  class remove final : public sync_impact {
  public:
    remove(sl::index id_entity) {
      m_id_entity = id_entity;
    }

    ~remove() final = default;

    void perform(access::world w) const final {
      w.remove(m_id_entity);
    }

  private:
    sl::index m_id_entity = 0;
  };
}

#endif

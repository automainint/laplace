/*  apps/quadwar/action/unit_move.h
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

#ifndef quadwar_action_unit_move_h
#define quadwar_action_unit_move_h

#include "../../../laplace/engine/basic_impact.h"
#include "../defs.h"

namespace quadwar_app::action {
  class unit_move : public engine::sync_impact {
  public:
    unit_move(size_t id_unit);
    ~unit_move() override = default;

    void perform(engine::access::world w) const override;

  private:
    size_t m_id_unit = {};
  };
}

#endif

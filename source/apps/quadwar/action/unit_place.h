/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef QUADWAR_ACTION_UNIT_PLACE_H
#define QUADWAR_ACTION_UNIT_PLACE_H

#include "../../../laplace/engine/basic_impact.h"
#include "../defs.h"

namespace quadwar_app::action {
  class unit_place : public engine::sync_impact {
  public:
    unit_place(sl::index id_unit) noexcept;
    ~unit_place() noexcept override = default;

    void perform(engine::access::world w) const noexcept override;

  private:
    sl::index m_id_unit = {};
  };
}

#endif

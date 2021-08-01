/*  apps/quadwar/action/pathmap_reset.h
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


#ifndef quadwar_action_pathmap_reset_h
#define quadwar_action_pathmap_reset_h

#include "../../../laplace/engine/basic_impact.h"
#include "../defs.h"

namespace quadwar_app::action {
  class pathmap_reset : public engine::sync_impact {
  public:
    pathmap_reset(sl::index id_pathmap);
    ~pathmap_reset() override = default;

    void perform(engine::access::world w) const override;

  private:
    sl::index m_id_pathmap = {};
  };
}

#endif

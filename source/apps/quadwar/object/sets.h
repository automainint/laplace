/*  apps/quadwar/object/sets.h
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

#ifndef quadwar_object_sets_h
#define quadwar_object_sets_h

#include "../../../laplace/engine/object/sets.h"

namespace quadwar_app::object::sets {
  using namespace laplace::engine::object::sets;

  enum set_id : sl::index {
    state_version = _native_count,

    root_is_loading,
    root_is_launched,
    root_launch,
    root_landscape,
    root_pathmap,
    root_slots,
    root_units,

    player_index,

    game_clock_time,

    land_width,
    land_height,

    unit_actor,
    unit_color,
    unit_health,
    unit_radius,
    unit_collision_radius,
    unit_movement_speed,
    unit_x,
    unit_y,
    unit_move_x,
    unit_move_y,
    unit_waypoint_index,
    unit_waypoint_count,

    pathmap_width,
    pathmap_height,

    _count
  };
}

#endif

target_sources(
  ${QUADWAR_OBJ}
    PRIVATE
      aqo_game_clock.cpp aqo_player.cpp aqo_landscape.cpp
      aqo_pathmap.cpp aqo_unit.cpp aqo_root.cpp
    PUBLIC
      sets.h unit.h pathmap.h landscape.h game_clock.h
      player.h root.h defs.h
)

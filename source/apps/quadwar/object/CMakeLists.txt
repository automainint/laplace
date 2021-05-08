target_sources(
  ${QUADWAR_OBJ}
    PRIVATE
      aqo_game_clock.cpp aqo_landscape.cpp aqo_pathmap.cpp
      aqo_player.cpp aqo_root.cpp aqo_unit.cpp
    PUBLIC
      defs.h game_clock.h landscape.h pathmap.h player.h
      root.h sets.h unit.h
)

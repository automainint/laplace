target_sources(
  ${QUADWAR_OBJ}
    PRIVATE
      aqo_player.cpp aqo_landscape.cpp aqo_pathmap.cpp
      aqo_unit.cpp aqo_root.cpp
    PUBLIC
      sets.h unit.h pathmap.h landscape.h player.h root.h
      defs.h)

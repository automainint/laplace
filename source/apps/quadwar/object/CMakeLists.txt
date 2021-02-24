target_sources(
  ${EXE_NAME}
    PRIVATE
      aqo_game_clock.cpp aqo_landscape.cpp aqo_player.cpp
      aqo_root.cpp
    PUBLIC
      defs.h game_clock.h landscape.h player.h root.h sets.h
)

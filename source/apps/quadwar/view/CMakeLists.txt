target_sources(
  ${QUADWAR_OBJ}
    PRIVATE
      aqv_units.cpp aqv_landscape.cpp aqv_game.cpp
      aqv_camera.cpp
    PUBLIC
      camera.h landscape.h game.h units.h defs.h
)

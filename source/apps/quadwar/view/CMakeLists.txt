target_sources(
  ${QUADWAR_OBJ}
    PRIVATE
      aqv_camera.cpp aqv_game.cpp aqv_landscape.cpp
      aqv_units.cpp
    PUBLIC
      camera.h defs.h game.h landscape.h units.h
)

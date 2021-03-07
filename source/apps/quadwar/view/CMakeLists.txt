target_sources(
  ${EXE_NAME}
    PRIVATE
      aqv_camera.cpp aqv_game.cpp aqv_landscape.cpp
    PUBLIC
      camera.h defs.h game.h landscape.h
)

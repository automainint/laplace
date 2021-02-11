target_sources(
  ${EXE_NAME}
    PRIVATE
      ex_entity.cpp ex_world.cpp
    PUBLIC
      entity.h mode.h ventity.h ventity.impl.h world.h
      world.impl.h world.predef.h
)

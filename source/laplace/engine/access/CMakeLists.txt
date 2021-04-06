target_sources(
  ${LAPLACE_OBJ}
    PRIVATE
      ex_entity.cpp ex_world.cpp
    PUBLIC
      entity.h mode.h world.h world.impl.h world.predef.h
)

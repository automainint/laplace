target_sources(
  ${LAPLACE_OBJ}
    PRIVATE
      ex_world.cpp ex_entity.cpp
    PUBLIC
      world.predef.h mode.h world.h world.impl.h entity.h)

target_sources(
  ${LAPLACE_EXE}
    PRIVATE
      ex_entity.cpp ex_world.cpp
    PUBLIC
      entity.h mode.h ventity.h ventity.impl.h world.h
      world.impl.h world.predef.h
)

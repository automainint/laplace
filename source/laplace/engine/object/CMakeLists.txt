target_sources(
  ${LAPLACE_OBJ}
    PRIVATE
      eo_actor.cpp eo_timer.cpp
    PUBLIC
      actor.h sets.h timer.h
)

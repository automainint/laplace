target_sources(
  ${LAPLACE_OBJ}
    PRIVATE
      eo_actor.cpp eo_sets.cpp eo_timer.cpp
    PUBLIC
      sets.h actor.h timer.h)

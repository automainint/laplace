target_sources(
  ${EXE_NAME}
    PRIVATE
       eo_actor.cpp eo_timer.cpp
    PUBLIC
       actor.h sets.h timer.h
)

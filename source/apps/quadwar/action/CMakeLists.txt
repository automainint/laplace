target_sources(
  ${QUADWAR_OBJ}
    PRIVATE
      aqa_pathmap_reset.cpp aqa_unit_move.cpp
      aqa_unit_place.cpp
    PUBLIC
      pathmap_reset.h unit_move.h unit_place.h
)

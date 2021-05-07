target_sources(
  ${QUADWAR_OBJ}
    PRIVATE
      aqp_qw_loading.cpp
    PUBLIC
      defs.h ids.h qw_init.h qw_launch.h qw_loading.h
      qw_player_name.h qw_slot_create.h qw_slot_remove.h
)

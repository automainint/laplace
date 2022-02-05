target_sources(
  ${QUADWAR_OBJ}
    PRIVATE
      aqp_qw_loading.cpp
    PUBLIC
      qw_loading.h qw_init.h ids.h qw_player_name.h
      qw_launch.h qw_slot_create.h qw_slot_remove.h qw_order_move.h defs.h)

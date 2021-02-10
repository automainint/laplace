target_sources(
  ${EXE_NAME}
    PRIVATE
       aq_lobby.cpp aq_main.cpp aq_mainmenu.cpp aq_player.cpp
      aq_quadwar.cpp aq_qw_factory.cpp aq_root.cpp aq_session.cpp
    PUBLIC
       ids.h landscape.h lobby.h mainmenu.h player.h quadwar.h
      qw_factory.h qw_player_name.h qw_slot_create.h qw_slot_remove.h root.h
      session.h sets.h
)

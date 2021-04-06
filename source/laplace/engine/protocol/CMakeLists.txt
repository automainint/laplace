target_sources(
  ${LAPLACE_OBJ}
    PRIVATE
      ep_debug.cpp
    PUBLIC
      basic_event.h client_desync.h debug.h ids.h ping.h
      public_key.h request_events.h server_clock.h server_idle.h
      server_seed.h slot_create.h slot_remove.h
)

target_sources(
  ${EXE_NAME}
    PRIVATE
       ep_debug.cpp
    PUBLIC
       basic_event.h client_ping.h debug.h ids.h prime.h
      public_key.h request_events.h server_clock.h server_idle.h
      server_seed.h slot_create.h slot_remove.h
)

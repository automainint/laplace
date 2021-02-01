target_sources(
  ${EXE_NAME}
    PRIVATE
       ep_debug.cpp ep_ids.cpp ep_slot_create.cpp
      ep_slot_remove.cpp
    PUBLIC
       client_ping.h debug.h elementary_events.h ids.h prime.h
      public_key.h request_events.h server_clock.h server_seed.h
      slot_create.h slot_remove.h
)

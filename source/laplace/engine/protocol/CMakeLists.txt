target_sources(
  ${LAPLACE_OBJ}
    PRIVATE
      ep_debug.cpp
    PUBLIC
      slot_remove.h session_request.h server_heartbeat.h ids.h
      basic_event.h session_response.h basic_value.h debug.h slot_create.h
      all.h session_token.h request_events.h)

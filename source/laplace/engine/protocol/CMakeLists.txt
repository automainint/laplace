target_sources(
  ${LAPLACE_OBJ}
    PRIVATE
      ep_debug.cpp
    PUBLIC
      all.h basic_event.h basic_value.h debug.h ids.h
      request_events.h server_idle.h session_request.h session_response.h
      session_token.h slot_create.h slot_remove.h
)

target_sources(
  ${LAPLACE_OBJ}
    PRIVATE
      n_session_request.test.cpp n_client_ready.test.cpp
      n_request_events.test.cpp n_session_token.test.cpp n_server_ping.test.cpp
      n_request_token.test.cpp n_slot_create.test.cpp n_server_init.test.cpp
      n_server_heartbeat.test.cpp n_server_seed.test.cpp n_server_pause.test.cpp
      n_slot_remove.test.cpp n_session_response.test.cpp n_server_quit.test.cpp
      n_server_loading.test.cpp n_server_action.test.cpp n_server_clock.test.cpp
      n_server_reserve.test.cpp n_client_enter.test.cpp n_client_leave.test.cpp
    PUBLIC
      helpers.test.h)

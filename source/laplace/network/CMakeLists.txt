target_sources(
  ${LAPLACE_OBJ}
    PRIVATE
      n_clock.cpp n_server.cpp n_utils.cpp n_stats.cpp
      n_defs.cpp n_pipe.cpp n_transfer.cpp n_udp_ipv4.cpp
      n_session_pool.cpp n_random.cpp n_udp_ipv4_node.cpp
    PUBLIC
      server.h server.impl.h utils.h udp_ipv4.h pipe.h stats.h
      random.impl.h udp_ipv4_node.h session_state.h transfer.h
      transfer.impl.h session_pool.h random.h defs.h clock.h)
add_subdirectory(crypto)
add_subdirectory(interface)

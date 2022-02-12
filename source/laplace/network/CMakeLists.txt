target_sources(
  ${LAPLACE_OBJ}
    PRIVATE
      n_clock.cpp n_server.cpp n_utils.cpp n_stats.cpp
      n_defs.cpp n_transfer.cpp n_udp_ipv4.cpp n_udp_ipv4_node.cpp
    PUBLIC
      server.h utils.h udp_ipv4.h stats.h udp_ipv4_node.h
      transfer.h defs.h clock.h)
add_subdirectory(crypto)
add_subdirectory(interface)

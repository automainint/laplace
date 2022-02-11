target_sources(
  ${LAPLACE_OBJ}
    PRIVATE
      n_clock.cpp n_server.cpp n_utils.cpp n_host.cpp
      n_defs.cpp n_transfer.cpp n_udp_ipv4.cpp n_udp_server.cpp
      n_udp_ipv4_node.cpp n_remote.cpp
    PUBLIC
      server.h udp_server.h utils.h udp_ipv4.h remote.h host.h
      udp_ipv4_node.h transfer.h defs.h clock.h)
add_subdirectory(crypto)
add_subdirectory(interface)

target_sources(
  ${LAPLACE_OBJ}
    PRIVATE
      n_server.cpp n_utils.cpp n_host.cpp n_defs.cpp
      n_transfer.cpp n_udp_server.cpp n_udp_ipv4_node.cpp n_remote.cpp
      udp_ipv4_interface.cpp
    PUBLIC
      server.h udp_server.h utils.h remote.h
      socket_interface.h host.h udp_ipv4_node.h transfer.h udp_ipv4_interface.h
      defs.h)
add_subdirectory(crypto)

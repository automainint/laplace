target_sources(
  ${LAPLACE_OBJ}
    PRIVATE
      n_server.cpp n_utils.cpp n_host.cpp n_defs.cpp
      n_transfer.cpp n_udp_server.cpp n_remote.cpp n_udp_node.cpp
    PUBLIC
      server.h udp_server.h udp_node.h utils.h remote.h host.h
      transfer.h defs.h
)
add_subdirectory(crypto)

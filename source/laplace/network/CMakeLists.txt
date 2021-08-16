target_sources(
  ${LAPLACE_OBJ}
    PRIVATE
      n_host.cpp n_remote.cpp n_server.cpp n_transfer.cpp
      n_udp_node.cpp n_udp_server.cpp n_utils.cpp
    PUBLIC
      defs.h host.h remote.h server.h transfer.h udp_node.h
      udp_server.h utils.h
)
add_subdirectory(crypto)

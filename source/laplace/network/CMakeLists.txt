target_sources(
  ${LAPLACE_OBJ}
    PRIVATE
      n_basic_tcp.cpp n_host.cpp n_remote.cpp n_server.cpp
      n_tcp_joint.cpp n_tcp_node.cpp n_transfer.cpp n_udp_node.cpp
      n_udp_server.cpp n_utils.cpp
    PUBLIC
      basic_tcp.h defs.h host.h remote.h server.h tcp_joint.h
      tcp_node.h transfer.h udp_node.h udp_server.h utils.h
)
add_subdirectory(crypto)

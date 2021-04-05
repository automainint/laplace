target_sources(
  ${LAPLACE_EXE}
    PRIVATE
      n_host.cpp n_remote.cpp n_server.cpp n_tcp_base.cpp
      n_tcp_joint.cpp n_tcp_node.cpp n_transfer.cpp n_udp_node.cpp
      n_udp_server.cpp n_utils.cpp
    PUBLIC
      defs.h host.h remote.h server.h tcp_base.h tcp_joint.h
      tcp_node.h transfer.h udp_node.h udp_server.h utils.h
)
add_subdirectory(crypto)

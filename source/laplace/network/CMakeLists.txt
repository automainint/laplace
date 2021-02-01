target_sources(
  ${EXE_NAME}
    PRIVATE
       n_base.cpp n_joint.cpp n_node.cpp n_udp_node.cpp
      n_utils.cpp
    PUBLIC
       base.h defs.h joint.h node.h udp_node.h utils.h
)

target_sources(
  ${LAPLACE_OBJ}
    PRIVATE
      c_family.test.cpp c_parser.test.cpp
      c_small_vector.test.cpp c_utils.test.cpp ee_astar.test.cpp ee_grid.test.cpp
      ee_hierarchical_grid.test.cpp ee_maze.test.cpp e_entity.test.cpp e_protocol.test.cpp
      e_world.test.cpp f_binary.test.cpp f_text.test.cpp m_basic.test.cpp
      m_matrix.test.cpp m_traits.test.cpp m_vector.test.cpp
      nc_ecc_rabbit.test.cpp nc_wolfssl.test.cpp n_server.test.cpp n_transfer.test.cpp
      n_udp.test.cpp ui_rect.test.cpp
)

target_sources(
  ${LAPLACE_OBJ}
    PRIVATE
      ee_maze.test.cpp f_text.test.cpp ee_grid.test.cpp
      n_transfer.test.cpp n_udp.test.cpp c_bytestream.test.cpp c_serial.test.cpp
      e_world.test.cpp c_embedded.test.cpp m_vector.test.cpp g_color.test.cpp
      e_entity.test.cpp n_server.test.cpp c_log.test.cpp e_protocol.test.cpp
      c_parser.test.cpp f_binary.test.cpp c_small_vector.test.cpp
      m_basic.test.cpp nc_ecc_rabbit.test.cpp nc_wolfssl.test.cpp
      ee_hierarchical_grid.test.cpp m_traits.test.cpp m_matrix.test.cpp ui_rect.test.cpp
      c_utf8.test.cpp c_utils.test.cpp ee_astar.test.cpp c_unival.test.cpp
      c_profiler.test.cpp
)

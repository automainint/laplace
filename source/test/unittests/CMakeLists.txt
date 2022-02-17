target_sources(
  ${LAPLACE_OBJ}
    PRIVATE
      e_solver.test.cpp ee_maze.test.cpp ni_log.test.cpp
      f_text.test.cpp uie_textedit.test.cpp c_string.test.cpp co_task.test.cpp
      ee_grid.test.cpp ui_basic_widget.test.cpp e3_quad.test.cpp
      n_udp_ipv4.test.cpp n_transfer.test.cpp c_bytestream.test.cpp
      c_socket.test.cpp c_serial.test.cpp e_world.test.cpp
      c_input_handler.test.cpp ni_protocol.test.cpp c_embedded.test.cpp
      e3_cylinder.test.cpp p_opengl.test.cpp n_random.test.cpp
      e3_intersection.test.cpp e3_polygon.test.cpp n_udp_ipv4_node.test.cpp
      ee_arithmetic.test.cpp m_vector.test.cpp g_color.test.cpp n_server.test.cpp
      n_clock.test.cpp c_static_shuffle.test.cpp e3_plane.test.cpp
      c_log.test.cpp e_basic_entity.test.cpp p_basic_input.test.cpp
      e_basic_factory.test.cpp e_protocol.test.cpp c_parser.test.cpp e3_box.test.cpp
      f_binary.test.cpp c_small_vector.test.cpp e3_triangle.test.cpp
      m_basic.test.cpp nc_ecc_rabbit.test.cpp nc_wolfssl.test.cpp
      ee_integral.test.cpp e3_vector.test.cpp co_generator.test.cpp
      ee_hierarchical_grid.test.cpp ui_layout.test.cpp m_traits.test.cpp m_matrix.test.cpp
      e3_sphere.test.cpp ui_rect.test.cpp n_slot_pool.test.cpp e_loader.test.cpp
      e3_ray.test.cpp ni_execution.test.cpp c_utf8.test.cpp c_utils.test.cpp
      ee_astar.test.cpp c_unival.test.cpp c_profiler.test.cpp)
add_subdirectory(protocol)

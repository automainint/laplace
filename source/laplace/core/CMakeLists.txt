target_sources(
  ${LAPLACE_OBJ}
    PRIVATE
      c_embedded.cpp c_family.cpp c_log.cpp c_options.cpp
      c_parser.cpp c_profiler.cpp c_serial.cpp c_socket.cpp c_string.cpp
      c_utf8.cpp
    PUBLIC
      bytestream.h defs.h embedded.h family.h family.impl.h
      input_handler.h input_handler.impl.h keys.h log.h options.h parser.h
      profiler.h serial.h serial.impl.h slib.h small_vector.h
      small_vector.impl.h socket.h string.h string.impl.h utf8.h utils.h
      utils.impl.h
)

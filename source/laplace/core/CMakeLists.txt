target_sources(
  ${LAPLACE_OBJ}
    PRIVATE
      c_socket.cpp c_embedded.cpp c_log.cpp c_parser.cpp
      c_profiler.cpp c_string.cpp c_utf8.cpp c_unival.cpp c_serial.cpp
    PUBLIC
      unival.h input_handler.impl.h string.h
      small_vector.impl.h utf8.h profiler.h socket.h utils.h bytestream.h
      small_vector.h unival.impl.h keys.h static_shaffle.h static_random.h
      static_random.impl.h serial.h embedded.h utils.impl.h string.impl.h slib.h
      log.h static_shaffle.impl.h parser.h input_handler.h
      serial.impl.h options.h defs.h)

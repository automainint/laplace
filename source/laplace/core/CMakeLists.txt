target_sources(
  ${LAPLACE_OBJ}
    PRIVATE
      c_embedded.cpp c_family.cpp c_log.cpp c_parser.cpp
      c_serial.cpp c_socket.cpp c_string.cpp c_utf8.cpp
    PUBLIC
      bytestream.h defs.h embedded.h family.h family.impl.h
      input_handler.h keys.h log.h options.h parser.h serial.h serial.impl.h
      slib.h small_vector.h small_vector.impl.h socket.h string.h
      string.impl.h utf8.h utils.h utils.impl.h
)

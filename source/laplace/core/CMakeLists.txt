target_sources(
  ${LAPLACE_EXE}
    PRIVATE
      c_element.cpp c_embedded.cpp c_family.cpp c_log.cpp
      c_object_manager.cpp c_parser.cpp c_pool.cpp c_string.cpp c_utf8.cpp
    PUBLIC
      bytestream.h defs.h element.h embedded.h family.h
      family.impl.h log.h object_manager.h object_manager.impl.h options.h
      parser.h pool.h serial.h serial.impl.h slib.h string.h
      string.impl.h utf8.h utils.h utils.impl.h
)

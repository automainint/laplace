target_sources(
  ${EXE_NAME}
    PRIVATE
       c_bytestream.cpp c_element.cpp c_embedded.cpp
      c_family.cpp c_object_manager.cpp c_parser.cpp c_pool.cpp c_utils.cpp
    PUBLIC
       bytestream.h defs.h element.h embedded.h family.h
      family.impl.h object_manager.h object_manager.impl.h parser.h pool.h
      utils.h utils.impl.h
)

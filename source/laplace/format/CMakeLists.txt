target_sources(
  ${LAPLACE_OBJ}
    PRIVATE
      f_binary.cpp f_buffer.cpp f_text.cpp f_utils.cpp
    PUBLIC
      binary.h buffer.h defs.h text.h utils.h
)

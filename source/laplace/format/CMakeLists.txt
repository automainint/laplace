target_sources(
  ${LAPLACE_OBJ}
    PRIVATE
      f_buffer.cpp f_text.cpp f_binary.cpp f_utils.cpp
    PUBLIC
      utils.h text.h buffer.h binary.h defs.h
)

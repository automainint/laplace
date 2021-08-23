target_sources(
  ${LAPLACE_OBJ}
    PRIVATE
      f_binary.cpp f_buffer.cpp f_image.cpp f_text.cpp
    PUBLIC
      binary.h buffer.h defs.h image.h text.h utils.h
      utils.impl.h
)

target_sources(
  ${LAPLACE_EXE}
    PRIVATE
      f_binary.cpp f_buffer.cpp f_image.cpp f_model.cpp
      f_text.cpp
    PUBLIC
      binary.h buffer.h defs.h image.h model.h text.h utils.h
      utils.impl.h
)

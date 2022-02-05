target_sources(
  ${LAPLACE_OBJ}
    PRIVATE
      f_text.cpp f_binary.cpp f_utils.cpp
    PUBLIC
      utils.h text.h binary.h defs.h)

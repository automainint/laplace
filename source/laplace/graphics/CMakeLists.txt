target_sources(
  ${LAPLACE_OBJ}
    PRIVATE
      g_color.cpp g_image.cpp g_program.cpp g_texture.cpp
      g_utils.cpp
    PUBLIC
      color.h defs.h image.h image.impl.h pixel.h pixel.impl.h
      program.h texture.h utils.h
)
add_subdirectory(flat)
add_subdirectory(tridi)

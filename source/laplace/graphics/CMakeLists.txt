target_sources(
  ${LAPLACE_OBJ}
    PRIVATE
      g_color.cpp g_program.cpp g_utils.cpp g_image.cpp
      g_texture.cpp
    PUBLIC
      image.impl.h pixel.h utils.h pixel.impl.h program.h
      color.h texture.h defs.h image.h)
add_subdirectory(flat)

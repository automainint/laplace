target_sources(
  ${LAPLACE_OBJ}
    PRIVATE
      g2_solid_shader.cpp g2_sprite_shader.cpp
      g2_framebuffer.cpp g2_solid_buffer.cpp g2_sprite_buffer.cpp
    PUBLIC
      framebuffer.h sprite_buffer.h solid_buffer.h
      solid_shader.h sprite_shader.h)

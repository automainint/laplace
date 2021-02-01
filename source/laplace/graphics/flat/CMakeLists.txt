target_sources(
  ${EXE_NAME}
    PRIVATE
       g2_framebuffer.cpp g2_solid_buffer.cpp
      g2_solid_shader.cpp g2_sprite_buffer.cpp g2_sprite_shader.cpp
    PUBLIC
       framebuffer.h solid_buffer.h solid_shader.h
      sprite_buffer.h sprite_shader.h
)

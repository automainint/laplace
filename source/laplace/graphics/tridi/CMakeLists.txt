target_sources(
  ${EXE_NAME}
    PRIVATE
      g3_mesh_buffer.cpp g3_mesh_shader.cpp
      g3_reflect_shader.cpp g3_refract_shader.cpp g3_shadow_shader.cpp
      g3_uvmap_shader.cpp
    PUBLIC
      mesh_buffer.h mesh_shader.h reflect_shader.h
      refract_shader.h shadow_shader.h uvmap_shader.h
)

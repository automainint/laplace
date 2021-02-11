target_sources(
  ${EXE_NAME}
    PRIVATE
      r_context.cpp r_material.cpp r_mesh.cpp r_model.cpp
      r_motion.cpp r_projection.cpp
    PUBLIC
      camera.h context.h defs.h instance.h light.h material.h
      mesh.h model.h motion.h projection.h scene.h track.h
      track.impl.h
)

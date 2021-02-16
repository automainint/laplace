target_sources(
  ${EXE_NAME}
    PRIVATE
      ee_geometry.cpp ee_shape.cpp
    PUBLIC
      geometry.h shape.h
)

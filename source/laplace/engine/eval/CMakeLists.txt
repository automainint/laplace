target_sources(
  ${EXE_NAME}
    PRIVATE
      ee_geometry.cpp ee_integral.cpp ee_shape.cpp
    PUBLIC
      geometry.h integral.h shape.h
)

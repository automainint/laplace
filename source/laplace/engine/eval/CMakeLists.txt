target_sources(
  ${EXE_NAME}
    PRIVATE
      ee_geometry.cpp ee_integral.cpp ee_random.cpp
      ee_shape.cpp
    PUBLIC
      geometry.h integral.h random.h shape.h
)

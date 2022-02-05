target_sources(
  ${LAPLACE_OBJ}
    PRIVATE
      e3_polygon.cpp e3_ray.cpp e3_plane.cpp e3_quad.cpp
      e3_triangle.cpp e3_intersection.cpp e3_vector.cpp e3_box.cpp
      e3_sphere.cpp e3_cylinder.cpp
    PUBLIC
      cylinder.h quad.h polygon.h vector.h box.h triangle.h
      sphere.h plane.h ray.h intersection.h)

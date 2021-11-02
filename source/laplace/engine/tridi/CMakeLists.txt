target_sources(
  ${LAPLACE_OBJ}
    PRIVATE
      ee_ray.cpp ee_intersection.cpp ee_triangle.cpp
      ee_box.cpp ee_cylinder.cpp ee_plane.cpp ee_polygon.cpp ee_shape.cpp
      ee_octree.cpp ee_sphere.cpp ee_length.cpp ee_quad.cpp
    PUBLIC
      cylinder.h quad.h length.h polygon.h box.h triangle.h
      sphere.h shape.h plane.h ray.h intersection.h octree.h
)

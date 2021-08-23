target_sources(
  ${LAPLACE_OBJ}
    PRIVATE
      ee_geometry.cpp ee_grid.cpp ee_integral.cpp ee_maze.cpp
      ee_random.cpp ee_shape.cpp
    PUBLIC
      astar.h astar.impl.h geometry.h grid.h integral.h
      integral.impl.h maze.h random.h shape.h
)

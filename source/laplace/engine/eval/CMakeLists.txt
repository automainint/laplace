target_sources(
  ${LAPLACE_OBJ}
    PRIVATE
      ee_maze.cpp ee_grid.cpp ee_random.cpp
      ee_hierarchical_grid.cpp ee_integral.cpp
    PUBLIC
      integral.impl.h astar.impl.h astar.h grid.h
      arithmetic.impl.h integral.h maze.h random.h hierarchical_grid.h)

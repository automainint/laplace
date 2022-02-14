target_sources(
  ${LAPLACE_OBJ}
    PRIVATE
      co_generator.cpp co_task.cpp co_utils.cpp
      co_iterator.cpp co_traits.cpp
    PUBLIC
      generator.h generator.impl.h utils.h traits.h
      utils.impl.h iterator.h iterator.impl.h task.h task.impl.h)

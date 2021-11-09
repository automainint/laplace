target_sources(
  ${LAPLACE_OBJ}
    PRIVATE
      e_basic_factory.cpp e_scheduler.cpp e_loader.cpp
      e_basic_entity.cpp e_solver.cpp e_world.cpp
    PUBLIC
      world.predef.h basic_factory.h helper.h
      eventorder.impl.h scheduler.h basic_impact.predef.h prime_impact.impl.h
      world.h basic_factory.impl.h basic_entity.h solver.h
      basic_entity.predef.h basic_impact.h basic_impact.impl.h prime_impact.h
      basic_entity.impl.h eventorder.h loader.h defs.h
)
add_subdirectory(access)
add_subdirectory(object)
add_subdirectory(action)
add_subdirectory(eval)
add_subdirectory(tridi)
add_subdirectory(protocol)

target_sources(
  ${LAPLACE_EXE}
    PRIVATE
      e_basic_entity.cpp e_basic_factory.cpp e_loader.cpp
      e_scheduler.cpp e_solver.cpp e_world.cpp
    PUBLIC
      basic_entity.h basic_entity.impl.h basic_entity.predef.h
      basic_factory.h basic_factory.impl.h basic_impact.h basic_impact.impl.h
      basic_impact.predef.h defs.h eventorder.h eventorder.impl.h helper.h loader.h
      prime_impact.h prime_impact.impl.h scheduler.h solver.h world.h
      world.predef.h
)
add_subdirectory(access)
add_subdirectory(action)
add_subdirectory(eval)
add_subdirectory(object)
add_subdirectory(protocol)

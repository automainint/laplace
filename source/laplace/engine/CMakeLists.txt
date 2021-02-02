target_sources(
  ${EXE_NAME}
    PRIVATE
       e_basic_entity.cpp e_basic_factory.cpp e_dh_rabbit.cpp
      e_geometry.cpp e_host.cpp e_remote.cpp e_scheduler.cpp e_server.cpp
      e_shape.cpp e_solver.cpp e_udp_server.cpp e_world.cpp
    PUBLIC
       basic_entity.h basic_entity.impl.h basic_entity.predef.h
      basic_factory.h basic_factory.impl.h basic_impact.h basic_impact.impl.h
      basic_impact.predef.h defs.h dh_rabbit.h eventorder.h eventorder.impl.h
      geometry.h host.h remote.h scheduler.h server.h shape.h solver.h
      udp_server.h world.h world.predef.h
)
add_subdirectory(access)
add_subdirectory(action)
add_subdirectory(object)
add_subdirectory(protocol)

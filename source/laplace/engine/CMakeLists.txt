target_sources(
  ${EXE_NAME}
    PRIVATE
       e_dh_rabbit.cpp e_entity.cpp e_factory.cpp
      e_geometry.cpp e_host.cpp e_impact.cpp e_remote.cpp e_scheduler.cpp
      e_server.cpp e_shape.cpp e_solver.cpp e_udp_server.cpp e_world.cpp
    PUBLIC
       defs.h dh_rabbit.h entity.h entity.impl.h
      entity.predef.h eventorder.h eventorder.impl.h factory.h factory.impl.h
      geometry.h host.h impact.h impact.impl.h impact.predef.h remote.h
      scheduler.h server.h shape.h solver.h udp_server.h world.h
      world.predef.h
)
add_subdirectory(access)
add_subdirectory(action)
add_subdirectory(object)
add_subdirectory(protocol)

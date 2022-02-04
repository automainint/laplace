target_sources(
  ${QUADWAR_OBJ}
    PRIVATE
      aq_main.cpp aq_session.cpp aq_quadwar.cpp
      aq_qw_factory.cpp
    PUBLIC
      qw_factory.h quadwar.h session.h defs.h)
add_subdirectory(object)
add_subdirectory(action)
add_subdirectory(ui)
add_subdirectory(view)
add_subdirectory(protocol)

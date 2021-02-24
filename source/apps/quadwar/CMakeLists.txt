target_sources(
  ${EXE_NAME}
    PRIVATE
      aq_main.cpp aq_quadwar.cpp aq_qw_factory.cpp
      aq_session.cpp
    PUBLIC
      defs.h quadwar.h qw_factory.h session.h
)
add_subdirectory(object)
add_subdirectory(protocol)
add_subdirectory(ui)

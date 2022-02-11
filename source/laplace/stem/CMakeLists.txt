target_sources(
  ${LAPLACE_OBJ}
    PRIVATE
      s_server.cpp s_application.cpp s_app_flat.cpp
      s_config.cpp
    PUBLIC
      server.h server.impl.h application.h app_flat.h config.h)
add_subdirectory(text)
add_subdirectory(ui)

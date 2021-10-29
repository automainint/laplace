target_sources(
  ${LAPLACE_OBJ}
    PRIVATE
      s_application.cpp s_app_flat.cpp s_config.cpp
    PUBLIC
      application.h app_flat.h config.h
)
add_subdirectory(text)
add_subdirectory(ui)

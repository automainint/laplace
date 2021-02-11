target_sources(
  ${EXE_NAME}
    PRIVATE
      s_application.cpp s_app_flat.cpp s_config.cpp
      s_native_api.cpp
    PUBLIC
      application.h app_flat.h config.h native_api.h
)

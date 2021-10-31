target_sources(
  ${LAPLACE_OBJ}
    PRIVATE
      p_opengl.cpp
    PUBLIC
      opengl.h thread.h events.h gldef.h wrap.h dummy.h
)
if(WIN32)
  add_subdirectory(win32)
endif()
if(UNIX AND (NOT APPLE))
  add_subdirectory(linux)
endif()

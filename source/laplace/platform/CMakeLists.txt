target_sources(
  ${LAPLACE_OBJ}
    PRIVATE
      p_opengl.cpp
    PUBLIC
      dummy.h events.h gldef.h opengl.h thread.h wrap.h
)
if(WIN32)
  add_subdirectory(win32)
endif()

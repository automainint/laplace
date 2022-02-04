target_sources(
  ${LAPLACE_OBJ}
    PRIVATE
      p_basic_input.cpp p_opengl.cpp
    PUBLIC
      opengl.h basic_input.h thread.h events.h gldef.h wrap.h
      dummy.h)
if(WIN32)
  add_subdirectory(win32)
endif()
if(UNIX AND (NOT APPLE))
  add_subdirectory(linux)
endif()
